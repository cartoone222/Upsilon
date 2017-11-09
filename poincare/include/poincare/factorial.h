#ifndef POINCARE_FACTORIAL_H
#define POINCARE_FACTORIAL_H

#include <poincare/static_hierarchy.h>
#include <poincare/evaluation_engine.h>

namespace Poincare {

class Factorial : public StaticHierarchy<1> {
public:
  Factorial(const Expression * argument, bool clone = true);
  Type type() const override;
  Expression * clone() const override;
private:
  constexpr static int k_maxOperandValue = 100;
  template<typename T> static Complex<T> computeOnComplex(const Complex<T> c, AngleUnit angleUnit);
  Complex<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::approximate<float>(this, context, angleUnit,computeOnComplex<float>);
  }
  Complex<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override {
    return EvaluationEngine::approximate<double>(this, context, angleUnit, computeOnComplex<double>);
  }
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override;
  int writeTextInBuffer(char * buffer, int bufferSize) const override;
  int simplificationOrderGreaterType(const Expression * e) const override;
  int simplificationOrderSameType(const Expression * e) const override;
};

}

#endif
