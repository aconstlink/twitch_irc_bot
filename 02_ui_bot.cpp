

#include <motor/profiling/probe_guard.hpp>

#include <motor/platform/global.h>
#include <motor/noise/permutation_table.h>

#include <motor/social/twitch/twitch_irc_client.h>
#include <motor/gfx/primitive/primitive_render_2d.h>

#include <motor/controls/types/ascii_keyboard.hpp>
#include <motor/controls/types/three_mouse.hpp>

#include <motor/log/global.h>
#include <motor/memory/global.h>
#include <motor/concurrent/global.h>

#include <future>

namespace this_file
{
    using namespace motor::core::types ;

    using clk_t = std::chrono::high_resolution_clock ;

    class my_app : public motor::application::app
    {
        motor_this_typedefs( my_app ) ;

        motor::io::database_mtr_t _db ;
        motor::social::twitch::twitch_irc_bot_mtr_t _bot ;
        motor::social::twitch::twitch_irc_bot::commands_t _comms ;

        uint_t const seed = 127436 ;
        uint_t const bit = 8 ;
        uint_t const mixes = 3 ;

        motor::noise::permutation_table_t _pt = 
            motor::noise::permutation_table_t( seed, bit, mixes );

        enum class prim_type
        {
            triangle,
            circle
        };

        struct spawn_command
        {
            prim_type t ;
            size_t num ;
            motor::math::vec2f_t pos ;
        };
        motor::vector< spawn_command > _scomms ;

        struct particle
        {
            prim_type t ;
            motor::math::vec2f_t pos ;
            float_t radius ;
        };
        motor::vector_pod< particle > _particles ;
        clk_t::time_point _tp ;

        motor::gfx::primitive_render_2d_t _pr ;

        motor::string_t _print_list ;

        //*********************************************************************************************
        virtual void_t on_init( void_t ) noexcept
        {
            _db = motor::shared( motor::io::database( motor::io::path_t( DATAPATH ), "./working", "data" ) ) ;

            {
                motor::application::window_info_t wi ;
                wi.x = 100 ;
                wi.y = 100 ;
                wi.w = 800 ;
                wi.h = 600 ;
                wi.gen = motor::application::graphics_generation::gen4_auto ;
                
                this_t::send_window_message( this_t::create_window( wi ), [&]( motor::application::app::window_view & wnd )
                {
                    wnd.send_message( motor::application::show_message( { true } ) ) ;
                    wnd.send_message( motor::application::cursor_message_t( {true} ) ) ;
                    wnd.send_message( motor::application::vsync_message_t( { true } ) ) ;
                } ) ;
            }

            _bot = motor::shared( motor::social::twitch::twitch_irc_bot( motor::move( _db ),
                motor::io::location_t( "twitch.bot_data.json" ) ) ) ;

            this->create_tcp_client( "twtich_irc_bot",
                motor::network::ipv4::binding_point_host { "6667", "irc.chat.twitch.tv" }, motor::share(_bot) ) ;

            _pr.init( "twitch_primitive_renderer" ) ;
        }

        //*********************************************************************************************
        virtual void_t on_event( window_id_t const wid, 
                motor::application::window_message_listener::state_vector_cref_t sv ) noexcept
        {
            if( sv.create_changed )
            {
                motor::log::global_t::status("[my_app] : window created") ;
            }
            if( sv.close_changed )
            {
                motor::log::global_t::status("[my_app] : window closed") ;
                this->close() ;
            }
        }

        //*********************************************************************************************
        virtual void_t on_update( motor::application::app::update_data_in_t ) noexcept 
        {
            if ( _bot->swap_commands( _comms ) )
            {
                for ( auto const & com : _comms )
                {
                    motor::log::global::status("arrived: " + com.name) ;
                    if( com.name == "spawn" )
                    {
                        auto number_check = [&]( motor::string_in_t s )
                        {
                            for( auto const c : s )
                            {
                                if( c < '0' || c > '9' ) return false ;
                            }
                            return true ;
                        } ;

                        if( !number_check( com.params[1] ) || 
                            !number_check( com.params[2] ) )
                        {
                            
                            _bot->send_response( com.user + ": Invalid param in command." ) ;
                            continue ;
                        }

                        this_t::spawn_command sc ;
                        if( com.params[0] == "triangle" )
                        {
                            sc.t = this_t::prim_type::triangle ;
                            sc.num = 1 ;
                            size_t const x = std::stol( com.params[1].c_str() ) ; 
                            size_t const y = std::stol( com.params[2].c_str() ) ; 
                            
                            _scomms.emplace_back( std::move( sc )  ) ;
                        }
                        else if( com.params[0] == "triangles" )
                        {
                            sc.t = this_t::prim_type::triangle ;
                            sc.num = std::min( size_t( std::stol( com.params[1].c_str() )), size_t(100) ) ;
                            _scomms.emplace_back( std::move( sc )  ) ;
                        }
                        else if ( com.params[ 0 ] == "circles" )
                        {
                            sc.t = this_t::prim_type::circle ;
                            sc.num = std::min( size_t( std::stol( com.params[ 1 ].c_str() ) ), size_t( 100 ) ) ;
                            _scomms.emplace_back( std::move( sc ) ) ;
                        }
                    }
                }
            }
        }

        //*********************************************************************************************
        virtual void_t on_graphics( motor::application::app::graphics_data_in_t ) noexcept 
        {
            // initialze particles
            if( _particles.size() == 0 )
            {
                for ( auto const & sc : _scomms )
                {
                    _particles.resize( sc.num ) ;
                    for( auto i=0; i<sc.num; ++i )
                    {
                        float_t const x = float_t( _pt.permute_at( uint_t( i ) ) ) / float_t( _pt.get_upper_bound() ) ;
                        float_t const y = float_t( _pt.permute_at( uint_t( i + sc.num ) ) ) / float_t( _pt.get_upper_bound() ) ;
                        
                        auto const pos = motor::math::vec2f_t( x * 2.0f - 1.0f, y  * 2.0f - 1.0f ) ;

                        float_t const r = std::min( float_t( _pt.permute_at( uint_t( i ) ) ) / float_t( _pt.get_upper_bound() ), 0.07f ) ;

                        _particles[i].t  = sc.t ;
                        _particles[i].pos = pos ;
                        _particles[i].radius = r ;
                    }
                }
                _scomms.clear() ;
                _tp = clk_t::now() ;
            }

            if( (clk_t::now() - _tp) > std::chrono::seconds(3) )
            {
                _particles.clear() ;
            }

            for( size_t i=0; i<_particles.size(); ++i )
            {
                if( _particles[i].t == this_t::prim_type::triangle )
                {
                    auto const r = _particles[i].radius ;

                    auto const p0 = _particles[i].pos + motor::math::vec2f_t( -r, -r) ;
                    auto const p1 = _particles[i].pos + motor::math::vec2f_t( 0.0f, r) ;
                    auto const p2 = _particles[i].pos + motor::math::vec2f_t( r, -r ) ;
                    _pr.draw_tri( 0, p0, p1, p2, motor::math::vec4f_t(0.8f, 0.7f, 0.5f, 1.0f ) ) ;
                }
                else if( _particles[i].t == this_t::prim_type::circle )
                {
                    auto const r = _particles[i].radius ;
                    auto const p0 = _particles[i].pos ;

                    _pr.draw_circle( 0, 10, p0, r, motor::math::vec4f_t(0.8f, 0.7f, 0.5f, 1.0f ),
                        motor::math::vec4f_t(0.8f, 0.7f, 0.5f, 1.0f ) ) ;
                }
            }

            _pr.prepare_for_rendering() ;
        } 

        //*********************************************************************************************
        virtual void_t on_render( this_t::window_id_t const, motor::graphics::gen4::frontend_ptr_t fr,
            motor::application::app::render_data_in_t rd ) noexcept 
        {
            if ( rd.first_frame )
            {
                _pr.configure( fr ) ;
            }

            _pr.prepare_for_rendering( fr ) ;
            _pr.render( fr, 0 ) ;
        }

        //*********************************************************************************************
        virtual bool_t on_tool( this_t::window_id_t const wid, motor::application::app::tool_data_ref_t ) noexcept 
        { 
            #if 0
            {
                bool_t show = false ;
                ImGui::ShowDemoWindow( &show ) ;
                ImPlot::ShowDemoWindow( &show ) ;
            }
            #endif
            return true ; 
        }

        //*********************************************************************************************
        virtual void_t on_shutdown( void_t ) noexcept 
        {
            motor::memory::release_ptr( _db ) ;
            motor::memory::release_ptr( _bot ) ;
        }
    };
}

int main( int argc, char ** argv )
{
    using namespace motor::core::types ;

    motor::application::carrier_mtr_t carrier = motor::platform::global_t::create_carrier(
        motor::shared( this_file::my_app() ) ) ;
    
    auto const ret = carrier->exec() ;
    
    motor::memory::release_ptr( carrier ) ;

    motor::io::global::deinit() ;
    motor::concurrent::global::deinit() ;
    motor::log::global::deinit() ;
    motor::profiling::global::deinit() ;
    motor::memory::global::dump_to_std() ;
    

    return ret ;
}