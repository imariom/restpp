namespace restpp {
    enum class http_method {
        GET, POST, DELETE, PATCH, OPTIONS, PUT
    };

    class response {
    public:
        void status();
        void headers();
        void body();
        void json();
        void type();
        void xml();
        void sent_request();
    };

    class request {};

    class headers {};

    class options{};

    class uri{
        std::string _path;
    public:
        uri(std::string path)
            : _path{path}
        {
        }
        operator std::string&() { return _path; }
        operator std::string() const { return _path; }
    };

    response fetch(uri _path, options _options = {});
    response fetch(request _request, options _options = {});
}

int main()
{
    //
    restpp::response response = restpp::fetch("http://example.com/movies.json");
    std::string jsonData = response.json();

    restpp::response response1 =
        restpp::fetch("http://example.com/movies.json",
            restpp::options{
                method: restpp::http_method::GET,
                body: nullptr, // binary or text data
                headers: {
                    {"Content-Type", "application/json"}
                }, // a list of
                signal: sig
            }
        );

    response.status();
    response.type();
    response.headers();
    response.body();
    response.json();
    response.xml();
    response.sent_request();
}