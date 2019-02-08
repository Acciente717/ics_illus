/**
 * Asynchronous Implementation of Proxy Lab with Boost Library
 * 
 * Author: Zhiyao Ma @ Peking University
 * Date: 2019/02/08
 * Version: 1.1
 * 
 * All rights reserved.
 */

#define BOOST_COROUTINES_NO_DEPRECATION_WARNING

#include <iostream>
#include <thread>
#include <vector>
#include <stdexcept>
#include <memory>
#include <string>
#include <algorithm>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

/**
 * Define abbreviation for long type names.
 */
using boost_socket = boost::asio::ip::tcp::socket;
using boost_acceptor = boost::asio::ip::tcp::acceptor;
using boost_error = boost::system::error_code;
using psocket = std::shared_ptr<boost_socket>;
using pacceptor = std::shared_ptr<boost::asio::ip::tcp::acceptor>;
using yield_context = boost::asio::yield_context;

/**
 * Define constants.
 */
constexpr unsigned long BUFF_SIZE = 1 << 20;

/**
 * Define global variables.
 * 
 * The approach below is better than defining them directly in
 * the global scope, especially when there are multiple source
 * files. KEEP IN MIND: the sequence of initialization of global
 * variables is not determined among source files. Thus, if there
 * is any dependancy between global variables ACROSS source files,
 * the approach below is a MUST.
 * 
 * For instance, the variable `work_guard` below depends on the
 * variable `io`. When `work_guard` is being initialized, it calls
 * the function `get_io_service`, which in turn guarantees that
 * `io` is initialized before any access.
 * 
 * Static variable inside any function is initialized at the first
 * access. But this does not provide any mutual exclusion. Two
 * threads simultaneously accessing the functions below when no one
 * has ever accessed before will trigger two initialization for the
 * static variable, which leads to RACE condition and undefined
 * behavior.
 * 
 * P.S.
 * The `inline` keyword is used to eliminate accessing overhead.
 * The `static` keyword is used to limit the scope of the name
 * within the file. If you are sharing global variables across
 * source files, do not add `static` keyword.
 * The `auto` keyword here frees us from typing long return types.
 * This is a feature available since C++14. (`auto` was first
 * introduced in C++11 though. However, you have to add something
 * called "trailing return type" after the function parameters,
 * which is just not so convenient if you are just using C++11
 * but not 14.)
 */
inline static auto &get_io_service()
{
    static boost::asio::io_service io;
    return io;
}

inline static auto &get_work_guard()
{
    static auto work_guard = boost::asio::make_work_guard(get_io_service());
    return work_guard;
}

inline static auto &get_threads()
{
    static std::vector<std::thread> threads;
    return threads;
}

/**
 * The line parser is used to extract lines from the HTTP request.
 * It reads bytes from the client, and returns a line in `std::string`
 * when the method `next_line` is called. An empty `std::string` will
 * be returned if we reach the EOF of client connection or any error
 * occurs.
 */
class line_parser
{
    psocket sock;
    std::vector<char> vec;
    std::string buffered_data;
public:
    explicit line_parser(psocket _sock) : sock(_sock), vec(BUFF_SIZE) {}

    std::string next_line(yield_context yield)
    {
        auto pos = buffered_data.find('\n');
        while (true)
        {
            if (pos != std::string::npos)
            {
                std::string ret_str(buffered_data.substr(0, pos + 1));
                buffered_data.erase(0, pos + 1);
                return ret_str;
            }
            std::size_t bytes_read;
            try
            {
                bytes_read = sock->async_read_some(boost::asio::buffer(vec), yield);
            }
            catch (boost::system::system_error &e)
            {
                if (e.code() == boost::asio::error::eof)
                {
                    std::string ret_str(std::move(buffered_data));
                    buffered_data.clear();
                    return ret_str;
                }
                buffered_data.clear();
                throw;
            }
            catch (...)
            {
                buffered_data.clear();
                throw;
            }
            pos = buffered_data.length();
            buffered_data.insert(buffered_data.end(), vec.data(), vec.data() + bytes_read);
            pos = buffered_data.find('\n', pos);
        }
    }
};

/**
 * The big for loop for all threads. Ignore all exceptions.
 */
static void worker(std::size_t thread_number)
{
    while (true)
    {
        try
        {
            get_io_service().run();
        }
        catch (std::exception &e)
        {
            std::cerr << "Exception caught at thread "
                      << thread_number << ": "
                      << e.what() << std::endl;
        }
        catch (...)
        {
            std::cerr << "Unknown exception caught at thread "
                      << thread_number << std::endl;
        }
        std::cerr << "Resume operation." << std::endl;
    }
}

/**
 * Create additional `pool_size` threads. They work along
 * with the main thread.
 */
static void create_threadpool(std::size_t pool_size)
{
    get_work_guard();
    for (std::size_t i = 1; i <= pool_size; ++i)
        get_threads().emplace_back(worker, i);
}

/**
 * Exeption class which represents error in parsing HTTP header.
 */
class client_header_error : public std::exception
{
public:
    virtual const char* what() const noexcept override
    {
        return "Bad HTTP header received from client.";
    }
};

/**
 * Patch the request received from the client.
 * Parse the URI from received URL.
 * Change "HTTP/1.1" to "HTTP/1.0".
 * Change "Connection: keep-alive" to "Connection: close"
 * Change "Proxy-Connection: keep-alive" to "Proxy-Connection: close"
 * 
 * Return `true` on success, `false` on error.
 */
static void patch_request(yield_context yield, line_parser &parser,
                          std::string &patched_req, std::string &host)
{
    {
        std::string first_line = parser.next_line(yield);
        auto len = first_line.length();
        std::string method, url;

        auto pos = first_line.find(' ', 0);
        if (pos == std::string::npos)
            throw client_header_error();
        method = first_line.substr(0, pos++);
        if (method != "GET")
            throw client_header_error();

        for (; pos < len; ++pos)
            if (first_line[pos] != ' ')
                break;
        if (pos >= len)
            throw client_header_error();
        auto next_pos = first_line.find(' ', pos);
        if (next_pos == std::string::npos)
            throw client_header_error();
        url = first_line.substr(pos, next_pos - pos);
        
        pos = url.find("//");
        if (pos != std::string::npos)
            pos += 2;
        else
            pos = 0;
        next_pos = url.find('/', pos);
        if (next_pos == std::string::npos)
            host = url, url = "/";
        else
            host = url.substr(pos, next_pos - pos), url = url.substr(next_pos, std::string::npos);

        patched_req = method + " " + url + " HTTP/1.0\r\n";
    }

    while (true)
    {
        std::string line = parser.next_line(yield);
        if (line.length() == 0)
            break;
        if (line == "\r\n")
        {
            patched_req += line;
            break;
        }
        if (line.find("Connection") == 0)
        {
            patched_req += "Connection: close\r\n";
            continue;
        }
        else if (line.find("Proxy-Connection") == 0)
        {
            patched_req += "Proxy-Connection: close\r\n";
            continue;
        }
        else
            patched_req += line;
    }
}

/**
 * Send everything received from the server back to the client.
 */
static void relay_back_to_client(yield_context yield, boost_socket &client_sock,
                                 boost_socket &server_sock)
{
    std::vector<char> vec(BUFF_SIZE);

    while (true)
    {
        std::size_t bytes_read;
        try
        {
            bytes_read = server_sock.async_read_some(boost::asio::buffer(vec), yield);
            boost::asio::async_write(client_sock, boost::asio::buffer(vec.data(), bytes_read), yield);
        }
        catch (boost::system::system_error &e)
        {
            if (e.code() != boost::asio::error::eof)
                throw;
            return;
        }
        catch (...)
        {
            return;
        }
    }
}

/**
 * Serve one connection from the client.
 */
static void proxy_routine(yield_context yield, psocket sock_ptr)
{
    std::string patched_req, host;
    try
    {
        // Parse and patch the request received from client.
        line_parser parser(sock_ptr);
        patch_request(yield, parser, patched_req, host);

        // Resolve the remote host, and try connecting to it.
        boost::asio::ip::tcp::resolver resolver(get_io_service());
        boost::asio::ip::tcp::resolver::query query(host, "80");
        auto result_iter = resolver.async_resolve(query, yield);
        decltype(result_iter) end_iter;
        boost_socket sock_to_server(get_io_service());
        boost::asio::async_connect(sock_to_server, result_iter, end_iter, yield);

        // Send the request to remote host. Relay everything back to the client.
        boost::asio::async_write(sock_to_server, boost::asio::buffer(patched_req), yield);
        relay_back_to_client(yield, *sock_ptr, sock_to_server);
    }
    catch (boost::system::system_error &e)
    {
        std::cerr << "Boost error caught at hostname: "
                  << host << std::endl
                  << "Error: " << e.what() << std::endl;
    }
    catch (client_header_error &e)
    {
        std::cerr << "Error occured when parsing HTTP header "
                  << "from the client" << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "System exception caught at hostname: "
                  << host << std::endl
                  << "Error: " << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception caught at hostname: "
                  << host << std::endl;
    }

    return;
}

/**
 * Handler for accepting a new connection.
 * Start a new asynchronous accept when done.
 */
static void accept_handler(const boost_error &ec, psocket sock_ptr,
                           pacceptor acceptor_ptr)
{
    // Accepted a new connection from the client. Serve it asynchronously.
    boost::asio::spawn(get_io_service(), [sock_ptr](yield_context yield) {
        proxy_routine(yield, sock_ptr);
    });

    // Start a new asynchronous accept.
    psocket new_sock_ptr = std::make_shared<boost_socket>(get_io_service());
    acceptor_ptr->async_accept(*new_sock_ptr,
                               [acceptor_ptr, new_sock_ptr](const boost_error &ec) {
                                   accept_handler(ec, new_sock_ptr, acceptor_ptr);
                               });
}

int main(int argc, char **argv)
{
    using namespace boost::asio;

    // Check arguments.
    if (argc < 2 || argc > 3)
    {
        std::cout << "Usage: ./proxy <port> <thread-number (Default: 1)>" << std::endl;
        return 1;
    }

    // Start the acceptor for accepting client connections.
    {
        pacceptor acceptor_ptr =
            std::make_shared<boost_acceptor>(get_io_service(),
                                             ip::tcp::endpoint(ip::tcp::v4(),
                                                               std::atoi(argv[1])));

        psocket sock_ptr = std::make_shared<boost_socket>(get_io_service());
        acceptor_ptr->async_accept(*sock_ptr,
                                   [acceptor_ptr, sock_ptr](const boost_error &ec) {
                                       accept_handler(ec, sock_ptr, acceptor_ptr);
                                   });
        
    }

    // Start additional threads if needed.
    int thread_num = 0;
    if (argc == 3)
    {
        thread_num = std::atoi(argv[2]) - 1;
        thread_num = std::max(0, thread_num);
        thread_num = std::min(256, thread_num);
    }
    create_threadpool(thread_num);
    
    // Start the main working thread.
    worker(0);

    return 0;
}
