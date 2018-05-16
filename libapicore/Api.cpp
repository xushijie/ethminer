#include "Api.h"

#include <boost/beast.hpp>
#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <cstdlib>
using namespace std;

Api::Api(const int &port, Farm &farm): m_farm(farm)
{
	int portNumber = port;
	bool readonly = true;

	// > 0 = rw, < 0 = ro, 0 = disabled
	if (portNumber > 0) {
		readonly = false;
	} else if(portNumber < 0) {
		portNumber = -portNumber;
	}

	if (portNumber > 0) {
		TcpSocketServer *conn = new TcpSocketServer("0.0.0.0", portNumber);
		this->m_server = new ApiServer(conn, JSONRPC_SERVER_V2, this->m_farm, readonly);
		this->m_server->StartListening();
	}
}


using tcp = boost::asio::ip::tcp;       // from <boost/asio/ip/tcp.hpp>
namespace http = boost::beast::http;    // from <boost/beast/http.hpp>

using namespace std;

/*
string HttpApi::remoteHost = Config::getInstance().getRemoteServer();
string HttpApi::port = Config::getInstance().getPort();
string HttpApi::url="/api/miner";
*/
namespace HttpApi{
        string remoteHost = "";
        string port = "";
}

void HttpApi::initialize(string configHost, string configPort)
{
        remoteHost = configHost;
        port = configPort;
}


bool HttpApi::postData(string json)
{
	try{
	    // The io_context is required for all I/O
        boost::asio::io_context ioc;

        // These objects perform our I/O
        tcp::resolver resolver{ioc};
        tcp::socket socket{ioc};

        //cout<<"Start resolve "<< host<<"   "<<port<<endl;
        // Look up the domain name
        auto const results = resolver.resolve(remoteHost, port);
        cout<<"Succeed in resolving "<<endl;
        // Make the connection on the IP address we get from a lookup
        boost::asio::connect(socket, results.begin(), results.end());

        http::request<http::string_body> req{http::verb::post, url, 11};
        req.set(http::field::host, remoteHost);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
        req.set(http::field::content_type, "application/json; charset=utf-8");
        //req.set(http::field::content_length, json.size());
		req.body() = json;
        req.prepare_payload();

        // Send the HTTP request to the remote host
        http::write(socket, req);

        // This buffer is used for reading and must be persisted
        boost::beast::flat_buffer buffer;

        // Declare a container to hold the response
        http::response<http::dynamic_body> res;

        // Receive the HTTP response
        http::read(socket, buffer, res);

        // Write the message to standard out
        std::cout << res << std::endl;

        // Gracefully close the socket
        boost::system::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes
        // so don't bother reporting it.
        //
        if(ec && ec != boost::system::errc::not_connected)
            //throw boost::system::system_error{ec};
			return false;

        // If we get here then the connection is closed gracefully
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }

	return true;
}

string JSonUtil::json2string(Json::Value&  root)
{
    /*Json::Value root;
    JsonSetter   setter(root);
    for_each(map.begin(), map.end(), setter); */
    Json::FastWriter fastWriter;
    std::string output = fastWriter.write(root);
    return output;
}
