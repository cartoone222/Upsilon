#include "tex_parser.h"
#include <ion/unicode/utf8_decoder.h>

namespace Reader {

// List of available Symbols
  static char const * k_SymbolsCommands[] = {
    "times", "div", "forall", "partial", "exists", "pm", "approx", "infty", "neq", "equiv", "leq", "geq", 
    "leftarrow", "uparrow", "rightarrow", "downarrow", "leftrightarrow", "updownarrow", "Leftarrow", "Uparrow", "Rightarrow", "Downarrow",
    "Alpha", "Beta", "Gamma", "Delta", "Epsilon", "Zeta", "Eta", "Theta", "Iota", "Kappa", "Lambda", 
    "Mu", "Nu", "Xi", "Omicron", "Pi", "Rho", "Sigma", "Tau", "Upsilon", "Phi", "Chi", "Psi","Omega",
    "alpha", "beta", "gamma", "delta", "epsilon", "zeta", "eta", "theta", "iota", "kappa", "lambda", 
    "mu", "nu", "xi", "omicron", "pi", "rho", "sigma", "tau", "upsilon", "phi", "chi", "psi", "omega",
  };

  //List of the available Symbol's CodePoints in the same order of the Symbol's list
  static uint32_t const k_SymbolsCodePoints[] = {
    0xd7, 0xf7, 0x2200, 0x2202, 0x2203, 0xb1, 0x2248, 0x221e, 0x2260, 0x2261, 0x2264, 0x2265, 
    0x2190, 0x2191, 0x2192, 0x2193, 0x2194, 0x2195, 0x21d0, 0x21d1, 0x21d2, 0x21d3, 
    0x391, 0x392, 0x393, 0x394, 0x395, 0x396, 0x397, 0x398, 0x399, 0x39a, 0x39b, 
    0x39c, 0x39d, 0x39e, 0x39f, 0x3a0, 0x3a1, 0x3a3, 0x3a4, 0x3a5, 0x3a6, 0x3a7, 0x3a8, 0x3a9, 
    0x3b1, 0x3b2, 0x3b3, 0x3b4, 0x3b5, 0x3b6, 0x3b7, 0x3b8, 0x3b9, 0x3ba, 0x3bb, 
    0x3bc, 0x3bd, 0x3be, 0x3bf, 0x3c0, 0x3c1, 0x3c3, 0x3c4, 0x3c5, 0x3c6, 0x3c7, 0x3c8, 0x3c9
  };
  
  // List of available Function Commands that don't require a specific handling
  static char const * k_FunctionCommands[] = {
    "arcos", "arcsin", "arctan", "arg", "cos", "cosh", "cot", "coth", 
    "csc", "deg", "det", "dim", "exp", "gcd", "hom", "inf", 
    "ker", "lg", "lim", "liminf", "limsup", "ln", "log", "max", 
    "min", "Pr", "sec", "sin", "sinh", "sup", "tan", "tanh"
  };

TexParser::TexParser(const char * text, const char * endOfText) : 
  m_text(text), 
  m_endOfText(endOfText),
  m_hasError(false)
{
    
}

Layout TexParser::getLayout() {
  Layout layout = popText(0);

  if (m_hasError) {
    return CodePointLayout::Builder(CodePoint(0xfffd));
  }

  return layout;
}

Layout TexParser::popBlock() {
  while (*m_text == ' ') {
    m_text ++;
  }

  if (*m_text == '{') {
    m_text ++;
    return popText('}');
  }

  if (*m_text == '\\') {
    m_text ++;
    return popCommand();
  }

  if (m_text >= m_endOfText) {
    m_hasError = true;
  }

  UTF8Decoder decoder(m_text);
  m_text ++;
  return CodePointLayout::Builder(decoder.nextCodePoint());
}

Layout TexParser::popText(char stop) {
  HorizontalLayout layout = HorizontalLayout::Builder();
  const char * start = m_text;
  
  while (m_text < m_endOfText && *m_text != stop) {
    switch (*m_text) {
      case '\\':
        if (start != m_text) {
          layout.addOrMergeChildAtIndex(LayoutHelper::String(start, m_text - start), layout.numberOfChildren(), false);
        }
        m_text ++;
        layout.addOrMergeChildAtIndex(popCommand(), layout.numberOfChildren(), false);
        start = m_text;
        break;
      case ' ':
        if (start != m_text) {
          layout.addOrMergeChildAtIndex(LayoutHelper::String(start, m_text - start), layout.numberOfChildren(), false);
        }
        m_text ++;
        start = m_text;
        break;
      case '^':
        if (start != m_text) {
          layout.addOrMergeChildAtIndex(LayoutHelper::String(start, m_text - start), layout.numberOfChildren(), false);
        }
        m_text ++;
        layout.addOrMergeChildAtIndex(VerticalOffsetLayout::Builder(popBlock(), VerticalOffsetLayoutNode::Position::Superscript), layout.numberOfChildren(), false);
        start = m_text;
        break;
      case '_':
        if (start != m_text) {
          layout.addOrMergeChildAtIndex(LayoutHelper::String(start, m_text - start), layout.numberOfChildren(), false);
        }
        m_text ++;
        layout.addOrMergeChildAtIndex(VerticalOffsetLayout::Builder(popBlock(), VerticalOffsetLayoutNode::Position::Subscript), layout.numberOfChildren(), false);
        start = m_text;
        break;
      default:
        m_text ++;
    }
  }

  if (start != m_text) {
    layout.addOrMergeChildAtIndex(LayoutHelper::String(start, m_text - start), layout.numberOfChildren(), false);
  }
  
  m_text ++;

  if (layout.numberOfChildren() == 1) {
    return layout.squashUnaryHierarchyInPlace();
  }

  return layout;
}

Layout TexParser::popCommand() {
  if (strncmp(k_fracCommand, m_text, strlen(k_fracCommand)) == 0) {
    m_text += strlen(k_fracCommand);
    if (*m_text == ' ' || *m_text == '{') {
      return popFracCommand();
    } 
  }
  else if (strncmp(k_sqrtCommand, m_text, strlen(k_sqrtCommand)) == 0) {
    m_text += strlen(k_sqrtCommand);
    if (*m_text == ' ' || *m_text == '{' || *m_text == '[') {
      return popSqrtCommand();
    }
  }

  for (int i = 0; i < k_NumberOfSymbols; i++) {
    if (strncmp(k_SymbolsCommands[i], m_text, strlen(k_SymbolsCommands[i])) == 0) {
      m_text += strlen(m_text);
      if (*m_text == ' ' || *m_text == '{') {
        return popSymbolCommand(i);
      }
    }
  }

  for (int i = 0; i < k_NumberOfFunctionCommands; i++) {
    if (strncmp(k_FunctionCommands[i], m_text, strlen(k_FunctionCommands[i])) == 0) {
      m_text += strlen(k_FunctionCommands[i]);
      if (*m_text == ' ' || *m_text == '(' || *m_text == '{') {
        return LayoutHelper::String(k_FunctionCommands[i], strlen(k_FunctionCommands[i]));
      }
    }
  }	

  m_hasError = true; 
  return LayoutHelper::String(m_text, strlen(m_text));
}

// Expressions
Layout TexParser::popFracCommand() {
  Layout numerator = popBlock();
  Layout denominator = popBlock();
  FractionLayout l = FractionLayout::Builder(numerator, denominator);
  return l;
}

Layout TexParser::popSqrtCommand() {
  while (*m_text == ' ') {
    m_text ++;
  }
  if (*m_text == '[') {
    m_text ++;
    Layout rootFactor = popText(']');
    Layout belowRoot = popBlock();
    return NthRootLayout::Builder(belowRoot, rootFactor);
  }
  else {
    return NthRootLayout::Builder(popBlock());
  }
}

Layout TexParser::popSymbolCommand(int SymbolIndex) {
  uint32_t codePoint = k_SymbolsCodePoints[SymbolIndex];
  return CodePointLayout::Builder(codePoint);
}


}