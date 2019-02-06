// Brief:  Generic visitor pattern implemented with template metaprogramming.
// Author: Caio Rodrigues
//------------------------------------------------------------------

#include <iostream>
#include <cstdio>
#include <string>
#include <memory>
#include <functional>

// Generic Visitor with CRTP (Curious Recurring Template)
// REMINDER: Template classes must always be
//           in header files (*.h or *.hpp)
template<typename Implementation>
class  Visitable{
public:
	/** Accept any visitor class which implements
	  * the method:
      * void Visitor::visit(Implementation& impl);
      *********************************************/
	template<typename Visitor>
	void accept(Visitor&& v) {
		v.visit(static_cast<Implementation&>(*this));
	}
};

// Shape interface - The base class must define the method accept
class IShape{
public:
	virtual ~IShape() = default;
};

// ============ Concrete shapes ======= //

class Circle: public IShape, public Visitable<Circle>{
public:
	double radius;
	Circle(double radius): radius(radius) { }
};

class Square: public IShape, public Visitable<Square>{
public:
	double side;
	Square(double side): side(side) { }
};

class Blob: public IShape, public Visitable<Blob>{
public:
	Blob(){}
};

//=======================================//

template<typename... Types>
class Visitor;

template<typename T>
class Visitor<T> {
public:
	template<typename Input> using Func = std::function<void (Input& sh)>;

	Func<T> _func;				
	Visitor(Func<T> func): _func(func){}
	
    virtual void visit(T & visitable){
		_func(visitable);
	}
};

template<typename T, typename... Types>
class Visitor<T, Types...> : public Visitor<Types...> {
public:
	template<typename Input> using Func = std::function<void (Input& sh)>;

	Func<T> _func;	

	Visitor(Func<T> func, Func<Types> ... funcn): _func(func), Visitor(funcn ...){}
	
    using Visitor<Types...>::visit;
    virtual void visit(T& visitable){ _func(visitable);	}
};


auto visitorX = Visitor<Circle, Square, Blob>{
	 [](Circle& s){  std::cout << "Shape is a circle" << "\n"; }
	,[](Square& s){ std::cout << "Shape is a square" << "\n"; }
    ,[](Blob& s){ std::cout << "Shape is a blob" << "\n"; }
};

// template<typename... Types>
// class Visitable {
// public:
//     virtual void accept(Visitor<Types...>& visitor) = 0;
// };



// ======== Generic Visitor =============//

// Note: This idea can be generalized even further by using variable
// templates and recursion.
template<class Result>
class FunctionAdapter{	
    // Lambda function are used for adding new behavior to the object.
	template<typename T> using Func = std::function<Result (T& sh)>;
	Result       _res;
	Func<Circle> _fn_circle;
	Func<Square> _fn_square;
	Func<Blob>   _fn_blob;
public:
	Result get(){ return _res; }
	FunctionAdapter(Func<Circle> fnCircle, Func<Square> fnSquare, Func<Blob> fnBlob)
		: _res{}
		, _fn_circle{fnCircle}
		, _fn_square{fnSquare}
		, _fn_blob{fnBlob}
	{
	}
	void visit(Circle& sh)  { _res = _fn_circle(sh); }
	void visit(Square& sh)  { _res = _fn_square(sh);}
	void visit(Blob& sh)    { _res = _fn_blob(sh); }

	template<class Visitable>
	Result operator()(Visitable& visitable){
		this->visit(visitable);
		return this->get();
	}
};

auto visitorGetName = FunctionAdapter<std::string>{
	[](Circle& ){ return "circle"; },
	[](Square& ){ return "square"; },
	[](Blob&   ){ return "blob";   },
};

// Creates operation to compute shape perimeter
auto visitorGetArea = FunctionAdapter<double>{
	[](Circle& s){ return 2 * 3.1415 * s.radius * s.radius ; },
	[](Square& s){ return 4.0 * s.side; },
	[](Blob&    ){ return -100.0;   },
};

int main()
{
	// Sample shapes
	auto s1 = Circle(3.0);
	auto s2 = Square(4.0);
	auto s3 = Blob();

#if 1
	std::cout << "===> Experiment 1: FunctionAdapter " << "\n";
	// Creates operation to get shape name as string

	s1.accept(visitorGetName);
	std::cout << "Type of shape 1 = " << visitorGetName.get() << "\n";
	std::cout << "Type of shape 2 = " << visitorGetName(s2) << "\n";
	std::cout << "Type of shape 3 = " << visitorGetName(s3) << "\n";

	std::cout << "===> Experiment 2: FunctionAdapter " << "\n";

	s1.accept(visitorGetArea);
	std::cout << "Perimeter of shape 1 = " << visitorGetArea.get() << "\n";
	s2.accept(visitorGetArea);
	std::cout << "Perimeter of shape 2 = " << visitorGetArea.get() << "\n";
	std::cout << "Perimeter of shape 2 = " << visitorGetArea(s2) << "\n";
	s3.accept(visitorGetArea);
	std::cout << "Perimeter of shape 3 = " << visitorGetArea.get() << "\n";
#endif

	return 0;
}