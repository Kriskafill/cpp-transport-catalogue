#include "svg.h"

namespace svg {

    using namespace std::literals;

    void Object::Render(const RenderContext& context) const {
        context.RenderIndent();

        // ���������� ����� ���� ����� ����������
        RenderObject(context);

        context.out << std::endl;
    }

    // ---------- Circle ------------------

    Circle& Circle::SetCenter(Point center) {
        center_ = center;
        return *this;
    }

    Circle& Circle::SetRadius(double radius) {
        radius_ = radius;
        return *this;
    }

    void Circle::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "  <circle"sv;

        out << " cx=\""sv << center_.x << "\""sv;
        out << " cy=\""sv << center_.y << "\""sv;
        out << " r=\""sv << radius_ << "\""sv;

        RenderAttrs(out);

        out << "/>"sv;
    }

    // ---------- Polyline ------------------

    Polyline& Polyline::AddPoint(Point point) {
        vertice_.push_back(std::move(point));
        return *this;
    }

    void Polyline::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "  <polyline points=\""sv;
        bool first = true;
        for (const Point& vert : vertice_) {
            if (first) first = false;
            else out.put(' ');
            out << vert.x << "," << vert.y;
        }
        out << "\""sv;

        RenderAttrs(out);

        out << "/>"sv;
    }

    // ---------- Text ------------------

    Text& Text::SetPosition(Point pos) {
        pos_ = std::move(pos);
        return *this;
    }

    Text& Text::SetOffset(Point offset) {
        offset_ = std::move(offset);
        return *this;
    }

    Text& Text::SetFontSize(uint32_t size) {
        size_ = std::move(size);
        return *this;
    }

    Text& Text::SetFontFamily(std::string font_family) {
        font_family_ = std::move(font_family);
        return *this;
    }

    Text& Text::SetFontWeight(std::string font_weight) {
        font_weight_ = std::move(font_weight);
        return *this;
    }

    Text& Text::SetData(std::string data) {
        data_ = std::move(data);
        return *this;
    }

    void Text::RenderObject(const RenderContext& context) const {
        auto& out = context.out;
        out << "  <text"sv;

        RenderAttrs(out);

        out << " x=\""sv << pos_.x << "\""sv;
        out << " y=\""sv << pos_.y << "\""sv;
        out << " dx=\""sv << offset_.x << "\""sv;
        out << " dy=\""sv << offset_.y << "\""sv;
        out << " font-size=\""sv << size_ << "\""sv;

        if (!font_family_.empty()) {
            out << " font-family=\""sv << font_family_ << "\""sv;
        }

        if (!font_weight_.empty()) {
            out << " font-weight=\""sv << font_weight_ << "\""sv;
        }

        out << ">"sv << data_ << "</text>"sv;
    }

    // ---------------Document-----------------

    void Document::AddPtr(std::unique_ptr<Object>&& obj) {
        objects_.emplace_back(std::move(obj));
    }

    void Document::Render(std::ostream& out) const {
        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n"sv;
        out << "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n"sv;

        for (const auto& obj : objects_) {
            obj->Render(out);
        }

        out << "</svg>"sv;
    }

}  // namespace svg