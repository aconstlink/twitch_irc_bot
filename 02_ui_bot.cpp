

#include <motor/profiling/probe_guard.hpp>

#include <motor/platform/global.h>

#include <motor/social/twitch/twitch_irc_client.h>

#include <motor/controls/types/ascii_keyboard.hpp>
#include <motor/controls/types/three_mouse.hpp>

#include <motor/log/global.h>
#include <motor/memory/global.h>
#include <motor/concurrent/global.h>

#include <future>

namespace this_file
{
    using namespace motor::core::types ;

    class my_app : public motor::application::app
    {
        motor_this_typedefs( my_app ) ;

        motor::io::database_mtr_t _db ;
        motor::social::twitch::twitch_irc_bot_mtr_t _bot ;
        motor::social::twitch::twitch_irc_bot::commands_t _comms ;

        motor::string_t _print_list ;

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

        }

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

        virtual bool_t on_tool( this_t::window_id_t const wid, motor::application::app::tool_data_ref_t ) noexcept 
        { 
            if ( _bot->swap_commands( _comms ) )
            {
                for ( auto const & com : _comms )
                {
                    _print_list += com.name + ", " ;
                }
                _comms.clear() ;
            }

            {
                if ( ImGui::Begin( "Commands" ) ) {}
                ImGui::Text( _print_list.c_str() ) ;
                ImGui::End() ;
            }

            #if 0
            {
                bool_t show = false ;
                ImGui::ShowDemoWindow( &show ) ;
                ImPlot::ShowDemoWindow( &show ) ;
            }
            #endif
            return true ; 
        }

        virtual void_t on_shutdown( void_t ) noexcept {}
    };
}

int main( int argc, char ** argv )
{
    using namespace motor::core::types ;

    motor::application::carrier_mtr_t carrier = motor::platform::global_t::create_carrier(
        motor::shared( this_file::my_app() ) ) ;
    
    auto const ret = carrier->exec() ;
    
    motor::memory::release_ptr( carrier ) ;

    motor::concurrent::global::deinit() ;
    motor::log::global::deinit() ;
    motor::profiling::global::deinit() ;
    motor::memory::global::dump_to_std() ;
    

    return ret ;
}