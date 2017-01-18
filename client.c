/*
    netcode.io reference implementation

    Copyright © 2016, The Network Protocol Company, Inc.

    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

        1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

        2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer 
           in the documentation and/or other materials provided with the distribution.

        3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived 
           from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
    USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <netcode.h>
#include <stdio.h>
#include <assert.h>

static uint8_t private_key[NETCODE_KEY_BYTES] = { 0x60, 0x6a, 0xbe, 0x6e, 0xc9, 0x19, 0x10, 0xea, 
                                                  0x9a, 0x65, 0x62, 0xf6, 0x6f, 0x2b, 0x30, 0xe4, 
                                                  0x43, 0x71, 0xd6, 0x2c, 0xd1, 0x99, 0x27, 0x26,
                                                  0x6b, 0x3c, 0x60, 0xf4, 0xb7, 0x15, 0xab, 0xa1 };

int main( int argc, char ** argv )
{
	(void) argc;
	(void) argv;

	if ( !netcode_init() )
    {
        printf( "error: failed to initialize netcode.io\n" );
        return 1;
    }

    double time = 0.0f;
	double delta_time = 0.1f;

	printf( "[client]\n" );

    struct netcode_client_t * client = netcode_client_create( time );

    if ( !client )
    {
        printf( "error: failed to create client\n" );
        return 1;
    }

    #define TEST_CONNECT_TOKEN_EXPIRY 30
    #define TEST_CLIENT_ID 1000
    #define TEST_PROTOCOL_ID 0x1122334455667788

    char * server_address = "[::1]:50000";

    uint8_t server_info[NETCODE_SERVER_INFO_BYTES];

    if ( !netcode_generate_server_info( 1, &server_address, TEST_CONNECT_TOKEN_EXPIRY, TEST_CLIENT_ID, TEST_PROTOCOL_ID, private_key, server_info ) )
    {
        printf( "error: failed to generate server info\n" );
        return 1;
    }

    netcode_client_connect( client, server_info );

	while ( 1 )
	{
		netcode_client_receive_packets( client );

		netcode_client_send_packets( client );

        if ( netcode_client_state( client ) <= NETCODE_CLIENT_STATE_DISCONNECTED )
            break;

		// todo: sleep

		time += delta_time;

		netcode_client_advance_time( client, time );
	}

    netcode_client_destroy( client );

    netcode_term();
	
    return 0;
}