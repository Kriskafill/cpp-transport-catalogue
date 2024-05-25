#include <iostream>

#include "json_reader.h"
#include "map_renderer.h"

// Sprint 10: Review Version #3

int main() {
    transport::transport_catalogue::TransportCatalogue catalogue;
    transport::json_reader::Reader reader(catalogue, std::cin);
    //transport::map_renderer::ReaderXML reader_xml(catalogue, reader.GetInfoXML());

    reader.Output(std::cout);
    //reader_xml.Output(std::cout);
}