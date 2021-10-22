#include "tex_parser.h"
#include <ion/unicode/utf8_decoder.h>

namespace Reader {

TexParser::TexParser(const char * text, const char * endOfText) : 
  m_text(text), 
  m_endOfText(endOfText),
  m_hasError(false)
{
    
}

Layout TexParser::getLayout() {
<<<<<<< HEAD
  HorizontalLayout layout = HorizontalLayout::Builder();
  const char * start = m_text;
  
  while (m_text < m_endOfText) {
    switch (*m_text)
    {
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
      m_text++;
      start = m_text;
      break;
    case '^':
      if (start != m_text) {
        layout.addOrMergeChildAtIndex(LayoutHelper::String(start, m_text - start), layout.numberOfChildren(), false);
      }
      m_text++;
      layout.addOrMergeChildAtIndex(popCommand(), layout.numberOfChildren(),false);
      start = m_text;
      break;
    default:
      m_text++;
      break;
    }
  }

  if (start != m_text) {
    layout.addOrMergeChildAtIndex(LayoutHelper::String(start, m_text - start), layout.numberOfChildren(), false);
  }
=======
  Layout layout = popText(0);
>>>>>>> 89c08acc7f59900fd12f2bdaacbae1827ccb73a1

  if (m_hasError) {
    return CodePointLayout::Builder(CodePoint(0xFFD));
  }

  return layout;
}

Layout TexParser::popBlock(char block) {
  while (*m_text == ' ') {
    m_text ++;
  }

  if (*m_text == block) {
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
        layout.addOrMergeChildAtIndex(popCommand(), layout.numberOfChildren(), false);
        start = m_text;
        break;
      default:
        m_text ++;
    }
  }

  if (start != m_text) {
    layout.addOrMergeChildAtIndex(LayoutHelper::String(start, m_text - start), layout.numberOfChildren(), false);
  }
  if (layout.numberOfChildren() == 1) {
    layout.squashUnaryHierarchyInPlace();
  }

  m_text ++;

  return layout;
}

Layout TexParser::popCommand() {
  if (strncmp(k_fracCommand, m_text, strlen(k_fracCommand)) == 0) {
    m_text += strlen(k_fracCommand);
    if (*m_text == ' ' || *m_text == '{') {
      return popFracCommand();
    } 
  }
  return popFracCommand();
}

Layout TexParser::popFracCommand() {
  Layout firstBlock = popBlock('{');
  Layout secondBlock = popBlock('{');
  return FractionLayout::Builder(firstBlock, secondBlock);
}

}
