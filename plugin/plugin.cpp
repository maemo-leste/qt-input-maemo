#include <QObject>
#include <QtPlugin>
#include <qpa/qplatforminputcontextplugin_p.h>
#include <qpa/qplatforminputcontext.h>

#include "input_context.h"

class QHPInputContextPlugin : public QPlatformInputContextPlugin {
Q_OBJECT
Q_PLUGIN_METADATA(IID QPlatformInputContextFactoryInterface_iid FILE "him.json")

public:
  QPlatformInputContext *create(const QString &key, const QStringList &paramList) override;
};

QPlatformInputContext *QHPInputContextPlugin::create(const QString &key, const QStringList &paramList) {
  Q_UNUSED(paramList);
  if (key.compare("him", Qt::CaseInsensitive) == 0) {
    return new QHildonInputContext();
  }
  return nullptr;
}

#include "plugin.moc"