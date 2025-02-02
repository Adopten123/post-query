#include <boost/asio.hpp>
#include <iostream>
#include <syncstream>
#include <thread>

namespace net = boost::asio;

template <typename Fn>
void RunWorkers(unsigned n, const Fn& fn) {
    n = std::max(1u, n);
    std::vector<std::jthread> workers;
    workers.reserve(n - 1);
    // Запускаем n-1 рабочих потоков, выполняющих функцию fn
    while (--n) {
        workers.emplace_back(fn);
    }
    fn();
}

int main() {
    using osync = std::osyncstream;

    net::io_context io{2};
    auto strand = net::make_strand(io);

    net::post(strand, [] { osync(std::cout) << "Cat, "; });

    net::post(strand, [] { osync(std::cout) << "Fish, "; });

    net::post(io, [] { osync(std::cout) << "Dog, "; });

    RunWorkers(2, [&io] {
        io.run();
    });

    std::cout << std::endl;
}