#pragma once

#include <iostream>
#include <map>
#include <string>
#include <variant>
#include <vector>

namespace json {

    class Node;
    using Dict = std::map<std::string, Node>;
    using Array = std::vector<Node>;
    using NodeType = std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict>;

    class ParsingError : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };

    class Node {
    public:

        Node() = default;

        Node(std::nullptr_t) : node_(nullptr) {}
        Node(int node) : node_(node) {}
        Node(double node) : node_(node) {}
        Node(bool node) : node_(node) {}
        Node(std::string node) : node_(node) {}
        Node(Array node) : node_(node) {}
        Node(Dict node) : node_(node) {}

        bool IsInt() const;
        bool IsDouble() const;
        bool IsPureDouble() const;
        bool IsBool() const;
        bool IsString() const;
        bool IsNull() const;
        bool IsArray() const;
        bool IsMap() const;

        int AsInt() const;
        bool AsBool() const;
        double AsDouble() const;
        const std::string& AsString() const;
        const Array& AsArray() const;
        const Dict& AsMap() const;

        const NodeType& GetNode() const { return node_; }
        friend std::ostream& operator<<(std::ostream& out, const Node& node);

    private:
        NodeType node_;
    };

    std::ostream& operator<<(std::ostream& out, const Node& node);

    inline bool operator==(const Node& left, const Node& right) {
        if (
            (left.IsInt() && right.IsInt()) ||
            (left.IsPureDouble() && right.IsPureDouble()) ||
            (left.IsBool() && right.IsBool()) ||
            (left.IsString() && right.IsString()) ||
            (left.IsNull() && right.IsNull()) ||
            (left.IsArray() && right.IsArray()) ||
            (left.IsMap() && right.IsMap())
            ) {
            if (left.GetNode() == right.GetNode()) return true;
        }

        return false;
    }

    inline bool operator!=(const Node& left, const Node& right) {
        return !(left == right);
    }

    class Document {
    public:

        explicit Document(Node root);

        const Node& GetRoot() const;

    private:
        Node root_;
    };

    inline bool operator==(const Document& left, const Document& right) {
        return left.GetRoot() == right.GetRoot();
    }

    inline bool operator!=(const Document& left, const Document& right) {
        return !(left == right);
    }

    Document Load(std::istream& input);

    void Print(const Document& doc, std::ostream& output);

}  // namespace json