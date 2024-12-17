#pragma once
#include <QCoreApplication>
#include <QAbstractNativeEventFilter>
#include <QDebug>
#include <QDataStream>

#include <xcb/xcb.h>
//#include <X11/Xlib.h>
//#include <X11/keysym.h>

class QHildonInputContext;
class QHildonEventFilter : public QAbstractNativeEventFilter
{
public:
  explicit QHildonEventFilter(QHildonInputContext* ctx);
  ~QHildonEventFilter() override = default;

  bool nativeEventFilter(const QByteArray &eventType, void *message, long *result) override;
private:
  QHildonInputContext* ctx;
};
