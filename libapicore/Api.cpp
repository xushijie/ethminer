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
#include <boost/lexical_cast.hpp>
#include <vector>

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
using udp = boost::asio::ip::udp;       // from <boost/asio/ip/tcp.hpp>
namespace ip = boost::asio::ip;
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


//https://stackoverflow.com/questions/212528/get-the-ip-address-of-the-machine/265978#265978
string HttpApi::getLocalIp(){
    try{
    boost::asio::io_service netService;
    udp::resolver   resolver(netService);
    udp::resolver::query query(udp::v4(), "www.baidu.com", "");
    udp::resolver::iterator endpoints = resolver.resolve(query);
    udp::endpoint ep = *endpoints;
    udp::socket socket(netService);
    socket.connect(ep);
    boost::asio::ip::address addr = socket.local_endpoint().address();
   /*
    boost::asio::io_service io_service;
    ip::tcp::socket s(io_service);

    using boost::lexical_cast;
    s.connect(
              ip::tcp::endpoint(ip::address::from_string(remoteHost), lexical_cast<int>(port)));
    string localIp = s.local_endpoint().address().to_string();
    */
    string localIp = addr.to_string();
   return localIp;
    }catch(std::exception& e){
        std::cerr<<"Failed to detect local IP: "<<e.what()<<endl;
        return "127.0.0.1";
    }
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
        //cout<<"Succeed in resolving "<<endl;
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

        // Gracefully close the socket
        boost::system::error_code ec;
        socket.shutdown(tcp::socket::shutdown_both, ec);

        // not_connected happens sometimes
        // so don't bother reporting it.
        //
        if(ec && ec != boost::system::errc::not_connected)
            //throw boost::system::system_error{ec};
			return false;
        std::ostringstream myos;
        myos << res << std::endl;
        if(!checkSuccess(myos.str())) return false;
        // If we get here then the connection is closed gracefully
    }
    catch(std::exception const& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return false;
    }

	return true;
}

std::string HttpApi::StripWhiteSpace(const std::string& s)
{
    if (s.size() == 0) return s;

    const static char* ws = " \t\v\n\r";
    size_t start = s.find_first_not_of(ws);
    size_t end = s.find_last_not_of(ws);
    if (start>s.size()) return std::string();
    return s.substr(start, end - start + 1);
}
StringVec HttpApi::SplitString(const std::string& str, char c)
{
    StringVec sv;
    std::string::size_type ilast = 0;
    std::string::size_type i = ilast;

    while ((i = str.find(c, i)) < str.size())
        {
            sv.push_back(str.substr(ilast, i - ilast));
            ilast = ++i;
        }
    sv.push_back(str.substr(ilast));

    return sv;
}


//! Split a string into a vector of substrings with \c c as a separator
/*! White space characters will be removed and the the strings will be converted to lowercase letters.
 */
StringVec HttpApi::SplitAndStripString(const std::string& str, char c)
{
    StringVec sv;
    std::string::size_type ilast = 0;
    std::string::size_type i = ilast;

    while ((i = str.find(c, i)) < str.size())
        {
            sv.push_back(StripWhiteSpace(str.substr(ilast, i - ilast)));
            ilast = ++i;
        }
    sv.push_back(str.substr(ilast));

    return sv;
}


bool  HttpApi::checkSuccess(const std::string input)
{

    StringVec sv = SplitAndStripString(input, '\n');

    StringVec FirstLine = SplitAndStripString(sv[0], ' ');
    if(FirstLine[1]=="200") return true;
    cout<<"Response: "<< input<<endl;
    /*
    size_t i = 1;
    std::map<std::string, std::string> fields;
    while (i < sv.size() && sv[i] != "")
    {
        StringVec field = SplitAndStripString(sv[i], ':');
        fields[field[0]] = field[1];
        ++i;
    }

    ++i;
    if(i<sv.size())
        return sv[i];

    return std::string();
    */
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
