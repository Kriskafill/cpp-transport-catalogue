#pragma once

#include <iostream>
#include <string>
#include <string_view>
#include <vector>

#include "transport_catalogue.h"

namespace transport {
    namespace input_reader {
        struct CommandDescription {

            explicit operator bool() const {
                return !command.empty();
            }
            bool operator!() const {
                return !operator bool();
            }

            std::string command;
            std::string id;
            std::string description;
        };

        struct Distance {
            std::string start_stop;
            std::string end_stop;
            int distance;
        };

        class InputReader {
        public:

            void ParseLine(std::string_view line);
            void ApplyCommands(transport_catalogue::TransportCatalogue& catalogue) const;

        private:
            std::vector<CommandDescription> commands_;
        };

        void ReadTransportCatalogue(transport_catalogue::TransportCatalogue& catalogue,
            InputReader& reader,
            std::istream& in);
    }
}