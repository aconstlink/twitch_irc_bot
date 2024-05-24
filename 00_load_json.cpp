
#include <motor/io/database.h>
#include <motor/log/global.h>

#include <nlohmann/json.hpp>

using namespace motor::core::types ;

namespace this_file
{
    struct login_data
    {
    public:

        motor::string_t broadcaster_name ;
        motor::string_t bot_name ;
        motor::string_t channel_name ;
        motor::string_t nick_name ;
        
        motor::string_t client_id ;
        motor::string_t client_secret ;
        motor::vector< motor::string_t >  scopes ;

    public:

        motor::string_t device_code ;
        motor::string_t access_token ;
        motor::string_t refresh_token ;
        motor::string_t bot_id ;
        motor::string_t broadcaster_id ;


    public:

        motor::string_t scopes_to_string( void_t ) const noexcept
        {
            motor::string_t ret ;
            for ( auto const & s : scopes )
            {
                ret += s + " " ;
            }
            return ret.substr( 0, ret.size() - 1 ) ;
        }
    };

    void_t load_data( login_data & ld, motor::io::location_in_t loc, motor::io::database_ref_t db ) noexcept
    {
        motor::string_t content ;

        // load file
        {
            db.load( loc ).wait_for_operation(
                [&] ( char_cptr_t buf, size_t const sib, motor::io::result const res )
            {
                if ( res == motor::io::result::ok )
                {
                    content = motor::string_t( buf, sib ) ;
                }
            } ) ;
        }

        // parse json
        if ( !content.empty() )
        {
            auto const json = nlohmann::json::parse( content ) ;

            if ( !json.contains( "required" ) )
            {
                motor::log::global_t::error( "missing required json object" ) ;
                exit( 1 ) ;
            }

            // handle required data
            {
                nlohmann::json const required = json[ "required" ] ;

                bool_t valid = true ;
                valid = valid || required.contains( "broadcaster_name" ) ;
                valid = valid || required.contains( "client_id" ) ;
                valid = valid || required.contains( "scopes" ) &&
                    required[ "scopes" ].is_array() ;

                if ( !valid )
                {
                    motor::log::global_t::error( "missing required json data" ) ;
                    exit( 1 ) ;
                }

                ld.client_id = required[ "client_id" ] ;
                ld.broadcaster_name = required[ "broadcaster_name" ] ;


                auto const scopes = required[ "scopes" ] ;
                ld.scopes.reserve( scopes.size() ) ;

                for ( auto const & s : scopes )
                {
                    if ( s.is_string() )
                    {
                        ld.scopes.emplace_back( motor::string_t( s ) ) ;
                    }
                }

                // print the scopes array as a string.
                {
                    motor::log::global::status( ld.scopes_to_string() ) ;
                }
            }

            if ( json.contains( "optional" ) )
            {
                nlohmann::json const optional = json[ "optional" ] ;

                if ( optional.contains( "bot_name" ) )
                {
                    ld.bot_name = optional[ "bot_name" ] ;
                }
                
                if( ld.bot_name.empty() )
                {
                    ld.bot_name = ld.broadcaster_name ;
                }

                if ( optional.contains( "channel_name" ) )
                {
                    ld.channel_name = optional[ "channel_name" ] ;
                }
                
                if( ld.channel_name.empty() )
                {
                    ld.channel_name = ld.broadcaster_name ;
                }

                if ( optional.contains( "nick_name" ) )
                {
                    ld.nick_name = optional[ "nick_name" ] ;
                }
                
                if( ld.nick_name.empty() )
                {
                    ld.nick_name = ld.broadcaster_name ;
                }

                if ( optional.contains( "client_secret" ) )
                {
                    ld.client_secret = optional[ "client_secret" ] ;
                }
            }
            else
            {
                ld.bot_name = ld.broadcaster_name ;
                ld.channel_name = ld.broadcaster_name ;
                ld.nick_name = ld.broadcaster_name ;
            }

            if ( json.contains( "internal" ) )
            {
                nlohmann::json const jdata = json[ "internal" ] ;

                if( jdata.contains("access_token") )
                    ld.access_token = jdata[ "access_token" ] ;
                if ( jdata.contains( "device_code" ) )
                    ld.device_code = jdata[ "device_code" ] ;
                if ( jdata.contains( "refresh_token" ) )
                    ld.refresh_token = jdata[ "refresh_token" ] ;
            }
        }
    }

    void_t store_data( login_data const & ld, motor::io::location_in_t loc, motor::io::database_ref_t db ) noexcept
    {
        nlohmann::json output_data = 
        {
            { 
                "required",   
                {
                    { "broadcaster_name", ld.broadcaster_name },
                    { "client_id", ld.client_id },
                    { "scopes", ld.scopes }
                } 
            },
            {
                "optional", 
                {
                    {"bot_name", ld.bot_name },
                    {"channel_name", ld.channel_name },
                    { "nick_name", ld.nick_name},
                    { "client_secret", ld.client_secret }
                }
            },
            {
                "internal",
                {
                    { "access_token", ld.access_token },
                    { "device_code", ld.device_code },
                    { "refresh_token", ld.refresh_token },
                }
            }
        } ;

        std::string dump = output_data.dump( 4 ) ;

        db.store( loc, dump.c_str(), dump.size() ).wait_for_operation( [&] ( motor::io::result const res )
        {
            if ( res == motor::io::result::ok )
            {
                motor::log::global_t::status( "Bot data written to : " + loc.as_string() ) ;
            }
            else
            {
                motor::log::global_t::error( "failed to write new tokens" ) ;
            }
        } ) ;
    }
}

int main( int argc, char ** argv )
{
    motor::io::database_t db = motor::io::database( motor::io::path_t( DATAPATH ), "./working", "data" ) ;

    this_file::login_data ld ;

    this_file::load_data( ld, motor::io::location_t( "twitch.credentials.tmpl" ), db ) ;
    this_file::store_data( ld, motor::io::location_t( "twitch.credentials.store" ), db ) ;

    

    
}