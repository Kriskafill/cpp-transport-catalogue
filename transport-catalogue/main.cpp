#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

// Sprint 9: Review Version #3

int main() {
    transport::transport_catalogue::TransportCatalogue catalogue;

    {
        transport::input_reader::InputReader reader;
        transport::input_reader::ReadTransportCatalogue(catalogue, reader, cin);
    }

    transport::stat_reader::DisplayTransportCatalogue(catalogue, cin, cout);
}