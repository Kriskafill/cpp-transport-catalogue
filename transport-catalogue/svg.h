#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <cstdint>
#include <deque>
#include <iostream>
#include <optional>
#include <memory>
#include <string>
#include <vector>
#include <variant>

namespace svg {

	struct Rgb {

		Rgb() = default;
		Rgb(uint8_t red, uint8_t green, uint8_t blue) : red(red), green(green), blue(blue) {}

		uint8_t red = 0;
		uint8_t green = 0;
		uint8_t blue = 0;
	};

	struct Rgba {

		Rgba() = default;
		Rgba(uint8_t red, uint8_t green, uint8_t blue, double opacity)
			: red(red), green(green), blue(blue), opacity(opacity) {}

		uint8_t red = 0;
		uint8_t green = 0;
		uint8_t blue = 0;
		double opacity = 1.0;
	};

	using Color = std::variant<std::monostate, std::string, Rgb, Rgba>;
	inline const Color NoneColor{ "none" };

	enum class StrokeLineCap {
		BUTT,
		ROUND,
		SQUARE,
	};

	enum class StrokeLineJoin {
		ARCS,
		BEVEL,
		MITER,
		MITER_CLIP,
		ROUND,
	};

	inline std::ostream& operator<<(std::ostream& out, const StrokeLineCap cap) {
		switch (cap) {
		case StrokeLineCap::BUTT:
			out << "butt";
			break;
		case StrokeLineCap::ROUND:
			out << "round";
			break;
		case StrokeLineCap::SQUARE:
			out << "square";
			break;
		}

		return out;
	}

	inline std::ostream& operator<<(std::ostream& out, const StrokeLineJoin join) {
		switch (join) {
		case StrokeLineJoin::ARCS:
			out << "arcs";
			break;
		case StrokeLineJoin::BEVEL:
			out << "bevel";
			break;
		case StrokeLineJoin::MITER:
			out << "miter";
			break;
		case StrokeLineJoin::MITER_CLIP:
			out << "miter-clip";
			break;
		case StrokeLineJoin::ROUND:
			out << "round";
			break;
		}

		return out;
	}

	inline std::ostream& operator<<(std::ostream& out, const std::monostate) {
		out << "none";
		return out;
	}

	inline std::ostream& operator<<(std::ostream& out, const Rgb& color) {
		out << "rgb(" << static_cast<int>(color.red) << ","
			<< static_cast<int>(color.green) << ","
			<< static_cast<int>(color.blue) << ")";
		return out;
	}

	inline std::ostream& operator<<(std::ostream& out, const Rgba& color) {
		out << "rgba(" << static_cast<int>(color.red) << ","
			<< static_cast<int>(color.green) << ","
			<< static_cast<int>(color.blue) << ","
			<< color.opacity << ")";
		return out;
	}

	inline std::ostream& operator<<(std::ostream& out, const Color& color) {

		std::visit([&out](const auto& value) {
			out << value;
			}, color);

		return out;
	}

	struct Point {

		Point() = default;
		Point(double x, double y) : x(x), y(y) {}

		double x = 0;
		double y = 0;
	};

	struct RenderContext {
		RenderContext(std::ostream& out)
			: out(out) {
		}

		RenderContext(std::ostream& out, int indent_step, int indent = 0)
			: out(out)
			, indent_step(indent_step)
			, indent(indent) {
		}

		RenderContext Indented() const {
			return { out, indent_step, indent + indent_step };
		}

		void RenderIndent() const {
			for (int i = 0; i < indent; ++i) {
				out.put(' ');
			}
		}

		std::ostream& out;
		int indent_step = 0;
		int indent = 0;
	};

	class Object {
	public:

		void Render(const RenderContext& context) const;
		virtual ~Object() = default;

	private:
		virtual void RenderObject(const RenderContext& context) const = 0;
	};

	template <typename Owner>
	class PathProps {
	public:

		Owner& SetFillColor(Color color) {
			fill_color_ = std::move(color);
			return AsOwner();
		}
		Owner& SetStrokeColor(Color color) {
			stroke_color_ = std::move(color);
			return AsOwner();
		}
		Owner& SetStrokeWidth(double width) {
			stroke_width_ = std::move(width);
			return AsOwner();
		}
		Owner& SetStrokeLineCap(StrokeLineCap line_cap) {
			stroke_linecap_ = std::move(line_cap);
			return AsOwner();
		}
		Owner& SetStrokeLineJoin(StrokeLineJoin line_join) {
			stroke_linejoin_ = std::move(line_join);
			return AsOwner();
		}

	protected:
		~PathProps() = default;

		void RenderAttrs(std::ostream& out) const {
			using namespace std::literals;

			if (fill_color_) {
				out << " fill=\""sv << *fill_color_ << "\""sv;
			}
			if (stroke_color_) {
				out << " stroke=\""sv << *stroke_color_ << "\""sv;
			}
			if (stroke_width_) {
				out << " stroke-width=\""sv << *stroke_width_ << "\""sv;
			}
			if (stroke_linecap_) {
				out << " stroke-linecap=\""sv << *stroke_linecap_ << "\""sv;
			}
			if (stroke_linejoin_) {
				out << " stroke-linejoin=\""sv << *stroke_linejoin_ << "\""sv;
			}
		}

	private:
		Owner& AsOwner() {
			return static_cast<Owner&>(*this);
		}

		std::optional<Color> fill_color_;
		std::optional<Color> stroke_color_;
		std::optional<double> stroke_width_;
		std::optional<StrokeLineCap> stroke_linecap_;
		std::optional<StrokeLineJoin> stroke_linejoin_;
	};

	class Circle final : public Object, public PathProps<Circle> {
	public:

		Circle& SetCenter(Point center);
		Circle& SetRadius(double radius);

	private:
		void RenderObject(const RenderContext& context) const override;

		Point center_;
		double radius_ = 1.0;
	};

	class Polyline final : public Object, public PathProps<Polyline> {
	public:

		Polyline& AddPoint(Point point);

	private:
		void RenderObject(const RenderContext& context) const override;

		std::vector<Point> vertice_;
	};

	class Text final : public Object, public PathProps<Text> {
	public:

		Text& SetPosition(Point pos);
		Text& SetOffset(Point offset);
		Text& SetFontSize(uint32_t size);
		Text& SetFontFamily(std::string font_family);
		Text& SetFontWeight(std::string font_weight);
		Text& SetData(std::string data);

	private:
		void RenderObject(const RenderContext& context) const override;

		Point pos_;
		Point offset_;
		uint32_t size_ = 1;
		std::string font_family_;
		std::string font_weight_;
		std::string data_;
	};

	class ObjectContainer {
	public:
		template <typename Obj> void Add(Obj obj);
		virtual void AddPtr(std::unique_ptr<Object>&& obj) = 0;
		virtual ~ObjectContainer() = default;
	};

	class Drawable {
	public:
		virtual void Draw(ObjectContainer& container) const = 0;
		virtual ~Drawable() = default;
	};

	class Document : public ObjectContainer {
	public:

		Document() = default;

		void AddPtr(std::unique_ptr<Object>&& obj) override;

		void Render(std::ostream& out) const;

	private:
		std::deque<std::unique_ptr<Object>> objects_;
	};

	template<typename Obj>
	void ObjectContainer::Add(Obj obj) {
		AddPtr(std::make_unique<Obj>(std::move(obj)));
	}

}  // namespace svg

namespace shapes {

	class Triangle : public ::svg::Drawable {
	public:
		Triangle(svg::Point p1, svg::Point p2, svg::Point p3)
			: p1_(p1)
			, p2_(p2)
			, p3_(p3) {
		}

		void Draw(svg::ObjectContainer& container) const {
			container.Add(svg::Polyline().AddPoint(p1_).AddPoint(p2_).AddPoint(p3_).AddPoint(p1_));
		}

	private:
		svg::Point p1_, p2_, p3_;
	};

	class Star : public svg::Drawable {
	public:

		Star(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
			polyline_ = CreateStar(center, outer_rad, inner_rad, num_rays);
		}

		void Draw(svg::ObjectContainer& container) const override {
			container.Add(polyline_);
		}

	private:
		svg::Polyline polyline_;

		svg::Polyline CreateStar(svg::Point center, double outer_rad, double inner_rad, int num_rays) {
			using namespace svg;
			Polyline polyline;
			for (int i = 0; i <= num_rays; ++i) {
				const double PI = 3.14159265358979323846;
				double angle = 2 * PI * (i % num_rays) / num_rays;
				polyline.AddPoint({ center.x + outer_rad * sin(angle), center.y - outer_rad * cos(angle) });
				if (i == num_rays) {
					break;
				}
				angle += PI / num_rays;
				polyline.AddPoint({ center.x + inner_rad * sin(angle), center.y - inner_rad * cos(angle) });
			}
			return polyline.SetFillColor("red").SetStrokeColor("black");
		}
	};

	class Snowman : public svg::Drawable {
	public:

		Snowman(svg::Point center, double radius) {
			c1_ = svg::Circle()
				.SetCenter({ center.x, center.y + 5 * radius })
				.SetRadius(2.0 * radius)
				.SetFillColor("rgb(240,240,240)")
				.SetStrokeColor("black");
			c2_ = svg::Circle()
				.SetCenter({ center.x, center.y + 2 * radius })
				.SetRadius(1.5 * radius)
				.SetFillColor("rgb(240,240,240)")
				.SetStrokeColor("black");
			c3_ = svg::Circle()
				.SetCenter(center)
				.SetRadius(radius)
				.SetFillColor("rgb(240,240,240)")
				.SetStrokeColor("black");
		}

		void Draw(svg::ObjectContainer& container) const override {
			container.Add(c1_);
			container.Add(c2_);
			container.Add(c3_);
		}

	private:
		svg::Circle c1_, c2_, c3_;
	};
}