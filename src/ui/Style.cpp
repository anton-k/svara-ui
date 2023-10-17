#include "Style.h"
#include "App.h"
#include "parser/Parser.h"

void setWidgetColor(HasStyle* widget, App* app, HasStyle::ColorId colId, Parser::Val<Parser::Col> col) {
  if (widget->hasColor(colId)) {
    app->setColor(col, [widget,colId] (auto c) {
      widget->setColor(colId, c);
    });
  } 
}

void setWidgetColor(HasStyle* widget, App* app, HasStyle::ColorId colId, std::optional<Parser::Val<Parser::Col>> col) {
  if (widget->hasColor(colId)) {
    app->setColor(col, [widget,colId] (auto c) {
      widget->setColor(colId, c);
    });
  } 
}

void HasStyle::setStyle(App* app, Parser::Style &style) {
  setWidgetColor(this, app, HasStyle::ColorId::First, style.color);
  setWidgetColor(this, app, HasStyle::ColorId::Second, style.secondaryColor);
  setWidgetColor(this, app, HasStyle::ColorId::Background, style.background);
  
  app->setJustificationType(style.textAlign, [this] (auto align) {
    this->setTextAlign(align);
  });
}
