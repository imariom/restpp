<img src="./assets/restpp.png" width="250" alt="restpp Logo">

# 

A modern C++ library for fetching resources and building RESTful APIs. **restpp** offers robust support for HTTP/HTTPS, synchronous and asynchronous operations, and advanced parsing capabilities, all with cross-platform compatibility.

## Table of Contents:
  - [Key Features](#key-features)
  - [Getting Started](#getting-started)
    - [Installing](#installing)
    - [Using CMake](#using-cmake)
    - [Fetching a remote resource](#fetching-a-remote-resource)
  - [Contributing](#contributing)
  - [License](#license)
  - [Contact](#contact)

# Key Features
  - HTTP and HTTPS support
  - Local and Remote Fetching
  - Synchronous and Asynchronous Modes
  - Request Cancellation
  - Built-in JSON Parsing
  - Built-in XML Parsing
  - Custom Type Parsing
  - **Platforms** - Windows, Linux, OS X, Unix, iOS, and Android

## Getting Started

### Installing

**Prerequisites**
  - A C++17 or newer compiler.
  - CMake version 3.15 or later.
  - Boost libraries (required).

**Installing with CMake**
Clone the repository and build restpp as a dependency for your project:

```bash
git clone https://github.com/imariom/restpp.git
cd restpp
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
sudo make install
```

Alternatively, add restpp as a submodule in your project:

```bash
git submodule add https://github.com/imariom/restpp.git external/restpp
```

Then include **restpp** in your CMakeLists.txt:

```cmake
add_subdirectory(external/restpp)
target_link_libraries(${PROJECT_NAME} PRIVATE restpp::fetch)
```

### Using CMake
To use from CMake:

```cmake
cmake_minimum_required(VERSION 3.9)
project(main)

find_package(restpp REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE restpp::fetch)
```

### Fetching a remote resource

```c++
#include <iostream>
#include <restpp/restpp.hpp>

int main()
{
    try {
        auto res = restpp::fetch("<resource-url>");

        std::cout << "Status code: " << res.status_code() << '\n';
        std::cout << "Headers: "     << res.headers() << '\n';
        std::cout << "Body: "        << res.body() << '\n';
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
    return 0;
}
```

## Contributing
Contributions are welcome! If you'd like to collaborate, please:
1. Fork the repository.
2. Create your feature branch (`git checkout -b feature/YourFeature`).
3. Commit your changes (`git commit -m 'Add YourFeature'`).
4. Push to the branch (`git push origin feature/YourFeature`).
5. Open a Pull Request.

Note: the RESTful APIs part will be implemented in the near future.

## License
Distributed under the MIT License. See `LICENSE` for more information.

## Contact
Mário Moiane - [connect@imariom.com](mailto:connect@imariom.com)
- Please visit my [website](https://imariom.com)
- Please use [Github issues](https://github.com/imariom/restpp) for filing bugs.
- Please follow me on Twitter [@__mrokok](https://x.com/__mrokok).
- LinkedIn Profile [Mário Moiane](https://www.linkedin.com/in/m%C3%A1rio-moiane-5aa424202)
