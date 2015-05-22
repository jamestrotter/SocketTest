#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>
#include <websocketpp/logger/syslog.hpp>
#include <websocketpp/server.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include <stdio.h>

typedef websocketpp::server<websocketpp::config::asio> server;
using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

using websocketpp::lib::thread;
using websocketpp::lib::mutex;
using websocketpp::lib::unique_lock;
using websocketpp::lib::condition_variable;

typedef server::message_ptr message_ptr;

enum action_type {
    SUBSCRIBE,
    UNSUBSCRIBE,
    MESSAGE
};

struct action {
    action(action_type t, connection_hdl h) : type(t), hdl(h) {}
    action(action_type t, connection_hdl h, server::message_ptr m)
      : type(t), hdl(h), msg(m) {}

    action_type type;
    websocketpp::connection_hdl hdl;
    server::message_ptr msg;
};

// Define a callback to handle incoming messages
void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
    std::cout << "on_message called with hdl: " << hdl.lock().get()
              << " and message: " << msg->get_payload()
              << std::endl;
	try
    {
		std::stringstream ss(msg->get_payload());
		boost::property_tree::ptree pt;
		boost::property_tree::read_json(ss, pt);

		BOOST_FOREACH(boost::property_tree::ptree::value_type &v, pt.get_child("Test"))
		{
			//assert(v.first.empty());
			std::cout << "TEST " << v.second.data() << std::endl;
		}
	}
	catch (std::exception const& e)
    {
        std::cerr << e.what() << std::endl;
    }
	
    try {
        s->send(hdl, msg->get_payload(), msg->get_opcode());
    } catch (const websocketpp::lib::error_code& e) {
        std::cout << "Echo failed because: " << e
                  << "(" << e.message() << ")" << std::endl;
    }
}

int main()
{
    printf("Starting Server...\n");
	uint16_t port = 8888;	
	server m_server;
	
	m_server.init_asio();
	
	m_server.set_message_handler(bind(&on_message,&m_server,::_1,::_2));
	
	m_server.listen(port);
	m_server.start_accept();
	
	try {
		m_server.run();
	} catch (const std::exception & e) {
		std::cout << e.what() << std::endl;
	}
    return 0;
}