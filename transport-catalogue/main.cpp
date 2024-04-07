#include <iostream>
#include <string>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

// Sprint 8: Review Version #2

int main() {
    transport::transport_catalogue::TransportCatalogue catalogue;

    int base_request_count;
    cin >> base_request_count >> ws;

    {
        transport::input_reader::InputReader reader;
        transport::input_reader::ReadTransportCatalogue(catalogue, reader, cin, base_request_count);
    }

    int stat_request_count;
    cin >> stat_request_count >> ws;
    transport::stat_reader::DisplayTransportCatalogue(catalogue, cin, cout, stat_request_count);
}