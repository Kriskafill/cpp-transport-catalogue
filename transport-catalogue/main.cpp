#include <iostream>

#include "json_reader.h"
#include "map_renderer.h"

// Sprint 12: Review Version #1

int main() {
    transport::transport_catalogue::TransportCatalogue catalogue;
    transport::json_reader::Reader reader(catalogue, std::cin);
    //transport::map_renderer::ReaderXML reader_xml(catalogue, reader.GetInfoXML());

    reader.Output(std::cout);
    //reader_xml.Output(std::cout);
}