#ifndef POINCARE_LEAST_COMMON_MULTIPLE_H
#define POINCARE_LEAST_COMMON_MULTIPLE_H

#include <poincare/layout_engine.h>
#include <poincare/static_hierarchy.h>

namespace Poincare {

class LeastCommonMultiple : public StaticHierarchy<2>  {
  using StaticHierarchy<2>::StaticHierarchy;
public:
  Type type() const override;
  Expression * clone() const override;
private:
  /* Layout */
  ExpressionLayout * privateCreateLayout(FloatDisplayMode floatDisplayMode, ComplexFormat complexFormat) const override {
    return LayoutEngine::createPrefixLayout(this, floatDisplayMode, complexFormat, name());
  }
  int writeTextInBuffer(char * buffer, int bufferSize) const override {
    return LayoutEngine::writePrefixExpressionTextInBuffer(this, buffer, bufferSize, name());
  }
  const char * name() const { return "lcm"; }
  /* Simplification */
  Expression * shallowReduce(Context& context, AngleUnit angleUnit) override;
  /* Evaluation */
  Complex<float> * privateEvaluate(SinglePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<float>(context, angleUnit); }
  Complex<double> * privateEvaluate(DoublePrecision p, Context& context, AngleUnit angleUnit) const override { return templatedEvaluate<double>(context, angleUnit); }
  template<typename T> Complex<T> * templatedEvaluate(Context& context, AngleUnit angleUnit) const;
};

}

#endif

