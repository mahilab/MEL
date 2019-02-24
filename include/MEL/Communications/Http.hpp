// MIT License
//
// MEL - Mechatronics Engine & Library
// Copyright (c) 2018 Mechatronics and Haptic Interfaces Lab - Rice University
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// This particular source file includes code which has been adapted from the
// following open-source projects (all external licenses attached at bottom):
//     SFML - Simple and Fast Multimedia Library
//
// Author(s): Evan Pezent (epezent@rice.edu)

#pragma once
#include <MEL/Communications/IpAddress.hpp>
#include <MEL/Communications/TcpSocket.hpp>
#include <MEL/Core/Time.hpp>
#include <MEL/Core/NonCopyable.hpp>
#include <map>
#include <string>

namespace mel {
/// A HTTP client
class Http : NonCopyable {
public:
    /// Define a HTTP request
    class Request {
    public:
        /// Enumerate the available HTTP methods for a request
        enum Method {
            Get,    ///< Request in get mode, standard method to retrieve a page
            Post,   ///< Request in post mode, usually to send data to a page
            Head,   ///< Request a page's header only
            Put,    ///< Request in put mode, useful for a REST API
            Delete  ///< Request in delete mode, useful for a REST API
        };

        /// Default constructor
        ///
        /// This constructor creates a GET request, with the root
        /// URI ("/") and an empty body.
        ///
        /// \param uri    Target URI
        /// \param method Method to use for the request
        /// \param body   Content of the request's body
        Request(const std::string& uri  = "/",
                Method method           = Get,
                const std::string& body = "");

        /// Set the value of a field
        ///
        /// The field is created if it doesn't exist. The name of
        /// the field is case-insensitive.
        /// By default, a request doesn't contain any field (but the
        /// mandatory fields are added later by the HTTP client when
        /// sending the request).
        ///
        /// \param field Name of the field to set
        /// \param value Value of the field
        void set_field(const std::string& field, const std::string& value);

        /// Set the request method
        ///
        /// See the Method enumeration for a complete list of all
        /// the availale methods.
        /// The method is Http::Request::Get by default.
        ///
        /// \param method Method to use for the request
        void set_method(Method method);

        /// Set the requested URI
        ///
        /// The URI is the resource (usually a web page or a file)
        /// that you want to get or post.
        /// The URI is "/" (the root page) by default.
        ///
        /// \param uri URI to request, relative to the host
        void set_uri(const std::string& uri);

        /// Set the HTTP version for the request
        ///
        /// The HTTP version is 1.0 by default.
        ///
        /// \param major Major HTTP version number
        /// \param minor Minor HTTP version number
        void set_http_version(unsigned int major, unsigned int minor);

        /// Set the body of the request
        ///
        /// The body of a request is optional and only makes sense
        /// for POST requests. It is ignored for all other methods.
        /// The body is empty by default.
        ///
        /// \param body Content of the body
        void set_body(const std::string& body);

    private:
        friend class Http;

        /// Prepare the final request to send to the server
        ///
        /// This is used internally by Http before sending the
        /// request to the web server.
        ///
        /// \return String containing the request, ready to be sent
        std::string prepare() const;

        /// Check if the request defines a field
        ///
        /// This function uses case-insensitive comparisons.
        ///
        /// \param field Name of the field to test
        ///
        /// \return True if the field exists, false otherwise
        bool has_field(const std::string& field) const;

        typedef std::map<std::string, std::string> FieldTable;

        FieldTable fields_;       ///< Fields of the header associated to their value
        Method method_;    ///< Method to use for the request
        std::string uri_;  ///< Target URI of the request
        unsigned int major_version_;  ///< Major HTTP version
        unsigned int minor_version_;  ///< Minor HTTP version
        std::string body_;           ///< Body of the request
    };

    /// Define a HTTP response
    class Response {
    public:
        /// Enumerate all the valid status codes for a response
        enum Status {
            // 2xx: success
            Ok = 200,        ///< Most common code returned when operation was
                             ///< successful
            Created  = 201,  ///< The resource has successfully been created
            Accepted = 202,  ///< The request has been accepted, but will be
                             ///< processed later by the server
            NoContent    = 204,  ///< The server didn't send any data in return
            ResetContent = 205,  ///< The server informs the client that it
                                 ///< should clear the view (form) that caused
                                 ///< the request to be sent
            PartialContent = 206,  ///< The server has sent a part of the
                                   ///< resource, as a response to a partial GET
                                   ///< request

            // 3xx: redirection
            MultipleChoices = 300,  ///< The requested page can be accessed from
                                    ///< several locations
            MovedPermanently = 301,  ///< The requested page has permanently
                                     ///< moved to a new location
            MovedTemporarily = 302,  ///< The requested page has temporarily
                                     ///< moved to a new location
            NotModified = 304,       ///< For conditional requests, means the
                                ///< requested page hasn't changed and doesn't
                                ///< need to be refreshed

            // 4xx: client error
            BadRequest = 400,    ///< The server couldn't understand the request
                                 ///< (syntax error)
            Unauthorized = 401,  ///< The requested page needs an authentication
                                 ///< to be accessed
            Forbidden = 403,  ///< The requested page cannot be accessed at all,
                              ///< even with authentication
            NotFound            = 404,  ///< The requested page doesn't exist
            RangeNotSatisfiable = 407,  ///< The server can't satisfy the
                                        ///< partial GET request (with a "Range"
                                        ///< header field)

            // 5xx: server error
            InternalServerError =
                500,  ///< The server encountered an unexpected error
            NotImplemented =
                501,  ///< The server doesn't implement a requested feature
            BadGateway = 502,  ///< The gateway server has received an error
                               ///< from the source server
            ServiceNotAvailable = 503,  ///< The server is temporarily
                                        ///< unavailable (overloaded, in
                                        ///< maintenance, ...)
            GatewayTimeout = 504,  ///< The gateway server couldn't receive a
                                   ///< response from the source server
            VersionNotSupported =
                505,  ///< The server doesn't support the requested HTTP version

            // 10xx: SFML custom codes
            InvalidResponse  = 1000,  ///< Response is not a valid HTTP one
            ConnectionFailed = 1001   ///< Connection with server failed
        };

        /// Default constructor
        ///
        /// Constructs an empty response.
        Response();

        /// Get the value of a field
        ///
        /// If the field \a field is not found in the response header,
        /// the empty string is returned. This function uses
        /// case-insensitive comparisons.
        ///
        /// \param field Name of the field to get
        ///
        /// \return Value of the field, or empty string if not found
        const std::string& get_field(const std::string& field) const;

        /// Get the response status code
        ///
        /// The status code should be the first thing to be checked
        /// after receiving a response, it defines whether it is a
        /// success, a failure or anything else (see the Status
        /// enumeration).
        ///
        /// \return Status code of the response
        Status get_status() const;

        /// Get the major HTTP version number of the response
        ///
        /// \return Major HTTP version number
        ///
        /// \see get_minor_http_version
        unsigned int get_major_http_version() const;

        /// Get the minor HTTP version number of the response
        ///
        /// \return Minor HTTP version number
        ///
        /// \see get_major_http_version
        unsigned int get_minor_http_version() const;

        /// Get the body of the response
        ///
        /// The body of a response may contain:
        /// \li the requested page (for GET requests)
        /// \li a response from the server (for POST requests)
        /// \li nothing (for HEAD requests)
        /// \li an error message (in case of an error)
        ///
        /// \return The response body
        const std::string& get_body() const;

    private:
        friend class Http;

        /// Construct the header from a response string
        ///
        /// This function is used by Http to build the response
        /// of a request.
        ///
        /// \param data Content of the response to parse
        void parse(const std::string& data);

        /// Read values passed in the answer header
        ///
        /// This function is used by Http to extract values passed
        /// in the response.
        ///
        /// \param in String stream containing the header values
        void parse_fields(std::istream& in);

        typedef std::map<std::string, std::string> FieldTable;

        FieldTable fields_;          ///< Fields of the header
        Status status_;              ///< Status code
        unsigned int major_version_;  ///< Major HTTP version
        unsigned int minor_version_;  ///< Minor HTTP version
        std::string body_;           ///< Body of the response
    };

    /// Default constructor
    Http();

    /// Construct the HTTP client with the target host
    ///
    /// This is equivalent to calling set_host(host, port).
    /// The port has a default value of 0, which means that the
    /// HTTP client will use the right port according to the
    /// protocol used (80 for HTTP). You should leave it like
    /// this unless you really need a port other than the
    /// standard one, or use an unknown protocol.
    ///
    /// \param host Web server to connect to
    /// \param port Port to use for connection
    Http(const std::string& host, unsigned short port = 0);

    /// Set the target host
    ///
    /// This function just stores the host address and port, it
    /// doesn't actually connect to it until you send a request.
    /// The port has a default value of 0, which means that the
    /// HTTP client will use the right port according to the
    /// protocol used (80 for HTTP). You should leave it like
    /// this unless you really need a port other than the
    /// standard one, or use an unknown protocol.
    ///
    /// \param host Web server to connect to
    /// \param port Port to use for connection
    void set_host(const std::string& host, unsigned short port = 0);

    /// Send a HTTP request and return the server's response.
    ///
    /// You must have a valid host before sending a request (see set_host).
    /// Any missing mandatory header field in the request will be added
    /// with an appropriate value.
    /// Warning: this function waits for the server's response and may
    /// not return instantly; use a thread if you don't want to block your
    /// application, or use a timeout to limit the time to wait. A value
    /// of Time::Zero means that the client will use the system default timeout
    /// (which is usually pretty long).
    ///
    /// \param request Request to send
    /// \param timeout Maximum time to wait
    ///
    /// \return Server's response
    Response send_request(const Request& request, Time timeout = Time::Zero);

private:
    TcpSocket connection_;  ///< Connection to the host
    IpAddress host_;        ///< Web host address
    std::string host_name_;  ///< Web host name
    unsigned short port_;   ///< Port used for connection with host
};

}  // namespace mel

/// \class mel::Http
/// \ingroup network
///
/// mel::Http is a very simple HTTP client that allows you
/// to communicate with a web server. You can retrieve
/// web pages, send data to an interactive resource,
/// download a remote file, etc. The HTTPS protocol is
/// not supported.
///
/// The HTTP client is split into 3 classes:
/// \li mel::Http::Request
/// \li mel::Http::Response
/// \li mel::Http
///
/// mel::Http::Request builds the request that will be
/// sent to the server. A request is made of:
/// \li a method (what you want to do)
/// \li a target URI (usually the name of the web page or file)
/// \li one or more header fields (options that you can pass to the server)
/// \li an optional body (for POST requests)
///
/// mel::Http::Response parse the response from the web server
/// and provides getters to read them. The response contains:
/// \li a status code
/// \li header fields (that may be answers to the ones that you requested)
/// \li a body, which contains the contents of the requested resource
///
/// mel::Http provides a simple function, SendRequest, to send a
/// mel::Http::Request and return the corresponding mel::Http::Response
/// from the server.
///
/// Usage example:
/// \code
/// // Create a new HTTP client
/// mel::Http http;
///
/// // We'll work on http://www.sfml-dev.org
/// http.set_host("http://www.sfml-dev.org");
///
/// // Prepare a request to get the 'features.php' page
/// mel::Http::Request request("features.php");
///
/// // Send the request
/// mel::Http::Response response = http.set_request(request);
///
/// // Check the status code and display the result
/// mel::Http::Response::Status status = response.get_status();
/// if (status == mel::Http::Response::Ok)
/// {
///     std::cout << response.get_body() << std::endl;
/// }
/// else
/// {
///     std::cout << "Error " << status << std::endl;
/// }
/// \endcode

//==============================================================================
// LICENSES
//==============================================================================

// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2017 Laurent Gomila (laurent@melml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
