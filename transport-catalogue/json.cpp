#include "json.h"

using namespace std;

namespace json {

	namespace {

		Node LoadNode(istream& input);

		Node LoadNull(istream& input) {
			auto it = std::istreambuf_iterator<char>(input);
			auto end = std::istreambuf_iterator<char>();
			for (int i = 0; i < 3; ++i) {
				if (it == end) {
					throw ParsingError("String parsing error");
				}
				++it;
			}

			if (it != end && isalnum(*it)) {
				throw ParsingError("String parsing error");
			}

			return Node{ nullptr };
		}

		Node LoadString(istream& input) {
			auto it = std::istreambuf_iterator<char>(input);
			auto end = std::istreambuf_iterator<char>();
			std::string s;
			while (true) {
				if (it == end) {
					throw ParsingError("String parsing error");
				}
				const char ch = *it;
				if (ch == '"') {
					++it;
					break;
				}
				else if (ch == '\\') {
					++it;
					if (it == end) {
						throw ParsingError("String parsing error");
					}
					const char escaped_char = *(it);

					switch (escaped_char) {
					case 'n':
						s.push_back('\n');
						break;
					case 't':
						s.push_back('\t');
						break;
					case 'r':
						s.push_back('\r');
						break;
					case '"':
						s.push_back('"');
						break;
					case '\\':
						s.push_back('\\');
						break;
					default:
						throw ParsingError("Unrecognized escape sequence \\"s + escaped_char);
					}
				}
				else if (ch == '\n' || ch == '\r') {
					throw ParsingError("Unexpected end of line"s);
				}
				else {
					s.push_back(ch);
				}
				++it;
			}

			return s;
		}

		Node LoadBool(istream& input, bool value) {
			auto it = std::istreambuf_iterator<char>(input);
			auto end = std::istreambuf_iterator<char>();
			for (int i = 0; i < 3; ++i) {
				if (it == end) {
					throw ParsingError("String parsing error");
				}
				++it;
			}
			if (!value) {
				if (it == end) {
					throw ParsingError("String parsing error");
				}
				++it;
			}

			if (it != end && isalnum(*it)) {
				throw ParsingError("String parsing error");
			}

			return Node{ value };
		}

		Node LoadArray(istream& input) {
			Array result;

			for (char c; input >> c;) {

				if (c == ']') {
					return Node(move(result));
				}

				if (c != ',') {
					input.putback(c);
				}

				result.push_back(LoadNode(input));

			}

			throw ParsingError("Failed to read number from stream"s);
		}

		Node LoadMap(istream& input) {
			Dict result;

			for (char c; input >> c;) {
				if (c == '}') {
					return Node(move(result));
				}

				if (c == ',') {
					input >> c;
				}

				string key = LoadString(input).AsString();
				input >> c;
				result.insert({ move(key), LoadNode(input) });
			}

			throw ParsingError("Failed to read number from stream"s);
		}

		using Number = variant<int, double>;
		Node LoadNumber(istream& input) {
			std::string parsed_num;

			// Считывает в parsed_num очередной символ из input
			auto read_char = [&parsed_num, &input] {
				parsed_num += static_cast<char>(input.get());
				if (!input) {
					throw ParsingError("Failed to read number from stream"s);
				}
				};

			// Считывает одну или более цифр в parsed_num из input
			auto read_digits = [&input, read_char] {
				if (!std::isdigit(input.peek())) {
					throw ParsingError("A digit is expected"s);
				}
				while (std::isdigit(input.peek())) {
					read_char();
				}
				};

			if (input.peek() == '-') {
				read_char();
			}

			if (input.peek() == '0') {
				read_char();
			}
			else {
				read_digits();
			}

			bool is_int = true;
			if (input.peek() == '.') {
				read_char();
				read_digits();
				is_int = false;
			}

			if (int ch = input.peek(); ch == 'e' || ch == 'E') {
				read_char();
				if (ch = input.peek(); ch == '+' || ch == '-') {
					read_char();
				}
				read_digits();
				is_int = false;
			}

			try {
				if (is_int) {
					try {
						return std::stoi(parsed_num);
					}
					catch (...) {
						// В случае неудачи, например, при переполнении,
						// код ниже попробует преобразовать строку в double
					}
				}
				return std::stod(parsed_num);
			}
			catch (...) {
				throw ParsingError("Failed to convert "s + parsed_num + " to number"s);
			}
		}

		Node LoadNode(istream& input) {
			char c;
			input >> c;

			if (c == 'n') return LoadNull(input);
			else if (c == '"') return LoadString(input);
			else if (c == 't') return LoadBool(input, true);
			else if (c == 'f') return LoadBool(input, false);
			else if (c == '[') return LoadArray(input);
			else if (c == '{') return LoadMap(input);
			else {
				input.putback(c);
				return LoadNumber(input);
			}

			return Node{ 0 };
		}

	}  // namespace  

	// NodePrint

	struct NodePrint {

		ostream& out;

		void operator()(nullptr_t) const {
			out << "null"sv;
		}

		void operator()(int node) const {
			out << node;
		}

		void operator()(double node) const {
			out << node;
		}

		void operator()(string node) const {
			out << "\""sv;

			for (char c : node) {
				if (c == '\r') {
					out << "\\r";
					continue;
				}

				if (c == '\n') {
					out << "\\n";
					continue;
				}

				if (c == '\t') {
					out << "\\t";
					continue;
				}

				if (c == '"' || c == '\\') out << '\\';
				out << c;
			}

			out << "\""sv;
		}

		void operator()(bool node) const {
			if (node) out << "true"sv;
			else out << "false"sv;
		}

		void operator()(Array node) const {
			out << "["sv;

			bool isFirst = true;
			for (const auto& val : node) {
				if (isFirst) isFirst = false;
				else out << ", "sv;

				out << val;
			}

			out << "]"sv;
		}

		void operator()(Dict node) const {
			out << "{"sv;

			bool isFirst = true;
			for (const auto& [key, value] : node) {
				if (isFirst) isFirst = false;
				else out << ", "sv;

				out << "\""sv << key << "\""sv << ": "sv << value;
			}

			out << "}"sv;
		}
	};

	ostream& operator<<(ostream& out, const Node& node) {
		visit(NodePrint{ out }, node.node_);
		return out;
	}

	// Is

	bool Node::IsInt() const {
		return holds_alternative<int>(node_);
	}

	bool Node::IsDouble() const {
		return holds_alternative<int>(node_) || holds_alternative<double>(node_);
	}

	bool Node::IsPureDouble() const {
		return holds_alternative<double>(node_);
	}

	bool Node::IsBool() const {
		return holds_alternative<bool>(node_);
	}

	bool Node::IsString() const {
		return holds_alternative<string>(node_);
	}

	bool Node::IsNull() const {
		return holds_alternative<nullptr_t>(node_);
	}

	bool Node::IsArray() const {
		return holds_alternative<Array>(node_);
	}

	bool Node::IsMap() const {
		return holds_alternative<Dict>(node_);
	}

	// Set

	int Node::AsInt() const {
		if (IsInt()) return get<int>(node_);
		else throw std::logic_error("Unright type");
	}

	bool Node::AsBool() const {
		if (IsBool()) return get<bool>(node_);
		else throw std::logic_error("Unright type");
	}

	double Node::AsDouble() const {
		if (IsInt()) return get<int>(node_);
		else if (IsDouble()) return get<double>(node_);
		else throw std::logic_error("Unright type");
	}

	const std::string& Node::AsString() const {
		if (IsString()) return get<string>(node_);
		else throw std::logic_error("Unright type");
	}

	const Array& Node::AsArray() const {
		if (IsArray()) return get<Array>(node_);
		else throw std::logic_error("Unright type");
	}

	const Dict& Node::AsMap() const {
		if (IsMap()) return get<Dict>(node_);
		else throw std::logic_error("Unright type");
	}

	// Document

	Document::Document(Node root)
		: root_(move(root)) {
	}

	const Node& Document::GetRoot() const {
		return root_;
	}

	Document Load(istream& input) {
		return Document{ LoadNode(input) };
	}

	void Print(const Document& doc, std::ostream& output) {
		(void)&doc;
		(void)&output;
		output << doc.GetRoot();
	}

}  // namespace json