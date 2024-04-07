#include "stat_reader.h"

#include <algorithm>
#include <iostream>

using namespace std::literals;

namespace transport {
	namespace stat_reader {
		namespace detail {
			void BusPrintStat(std::string_view request,
				std::ostream& output,
				std::tuple<size_t, size_t, double> info) {

				output << request << ": "s;

				if (std::get<0>(info) == 0) {
					output << "not found"s << std::endl;
				}
				else {
					output << std::get<0>(info) << " stops on route, "s
						<< std::get<1>(info) << " unique stops, "s
						<< std::get<2>(info) << " route length"s
						<< std::endl;
				}
			}

			void StopPrintStat(std::string_view request,
				std::ostream& output,
				std::optional<std::deque<std::string_view>> info) {

				output << request << ": "s;

				if (!info.has_value()) {
					output << "not found"s << std::endl;
				}
				else if (info.value().empty()) {
					output << "no buses"s << std::endl;
				}
				else {
					std::sort(info.value().begin(), info.value().end());
					output << "buses"s;
					for (const auto bus : info.value())
					{
						output << " "s << bus;
					}
					output << std::endl;
				}
			}
		}

		void ParseAndPrintStat(const transport_catalogue::TransportCatalogue& transport_catalogue,
			std::string_view request,
			std::ostream& output) {

			auto space_pos = request.find(' ');
			if (space_pos == request.npos) return;

			std::string str = std::string(request.substr(0, space_pos));
			if (str == "Bus") {
				auto info = transport_catalogue.GetBusInfo(std::string(request.substr(space_pos + 1)));
				detail::BusPrintStat(request, output, info);
			}
			else if (str == "Stop") {
				auto info = transport_catalogue.FindStop(std::string(request.substr(space_pos + 1)));
				if (info) {
					detail::StopPrintStat(request, output, info->buses_for_stop);
				}
				else {
					detail::StopPrintStat(request, output, std::nullopt);
				}
			}
		}
	}
}