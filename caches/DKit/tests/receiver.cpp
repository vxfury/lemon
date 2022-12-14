//
// receiver.cpp
// ~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <iostream>
#include <string>

#include <boost/asio.hpp>
#include <boost/bind.hpp>

const short multicast_port = 30001;

using namespace boost::asio;

class receiver {
  public:
    receiver(io_service &io_service, const ip::address &listen_address, const ip::address &multicast_address)
        : socket_(io_service)
    {
        // Create the socket so that multiple may be bound to the same address.
        ip::udp::endpoint listen_endpoint(listen_address, multicast_port);
        socket_.open(listen_endpoint.protocol());
        socket_.set_option(ip::udp::socket::reuse_address(true));
        socket_.bind(listen_endpoint);

        // Join the multicast group.
        socket_.set_option(ip::multicast::join_group(multicast_address));

        socket_.async_receive_from(
            buffer(data_, max_length), sender_endpoint_,
            boost::bind(&receiver::handle_receive_from, this, placeholders::error, placeholders::bytes_transferred));
    }

    void handle_receive_from(const boost::system::error_code &error, size_t bytes_recvd)
    {
        if (!error) {
            std::cout.write(data_, bytes_recvd);
            std::cout << std::endl;
            socket_.async_receive_from(buffer(data_, max_length), sender_endpoint_,
                                       boost::bind(&receiver::handle_receive_from, this, placeholders::error,
                                                   placeholders::bytes_transferred));
        }
    }

  private:
    enum { max_length = 1024 };
    ip::udp::socket socket_;
    ip::udp::endpoint sender_endpoint_;
    char data_[max_length];
};

int main(int argc, char *argv[])
{
    try {
        if (argc != 3) {
            std::cerr << "Usage: receiver <listen_address> <multicast_address>\n";
            std::cerr << "  For IPv4, try:\n";
            std::cerr << "    receiver 0.0.0.0 239.255.0.1\n";
            std::cerr << "  For IPv6, try:\n";
            std::cerr << "    receiver 0::0 ff31::8000:1234\n";
            return 1;
        }

        io_service io_service;
        receiver r(io_service, ip::address::from_string(argv[1]), ip::address::from_string(argv[2]));
        io_service.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
