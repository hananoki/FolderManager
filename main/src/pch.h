
#if defined __cplusplus

#include <QApplication> // Qt includes
#include <QDebug>
#include <QMessageBox>
#include <QDesktopWidget>

#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QMenu>
#include <QListWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QListView>
#include <QHeaderView>

#include <QFile>
#include <QTextStream>
#include <QSettings>
#include <QAction>

#include <QStyleFactory>
#include <QDir>
#include <QDirIterator>
#include <QClipboard>

#include <QUrl>
#include <QDesktopServices>
#include <QFileInfo>
#include <QDateTime>
#include <QToolBar>
#include <QStyle>
#include <QSize>
#include <QLayout>
#include <QDrag>
#include <QValidator>
#include <QColorDialog>
#include <QFileDialog>

#include <QMap>
#include <QProcess>
#include <QtConcurrent>
#include <QtConcurrent/QtConcurrentRun>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDragEnterEvent>
#include <QDragMoveEvent>



#include <functional>

//#include <zlib.h>

#include "Qtlib.h"

#include "Config.h"
#include "UIStatusBar.h"

#define ICON_EXPLORER icon::get( QString( "C:/Windows/explorer.exe" ) )

#define ICON_FOLDER QIcon( QString( ":/res/icon/folder.png" ) )


#define ICON_REFRESH QIcon( QString( ":/res/icon/reload.png" ) )
#define ICON_SETTINGS QIcon( QString( ":/res/icon/settings.png" ) )
#define ICON_EXEC QIcon( QString( ":/res/icon/icon_159120_16.png" ) )
#define ICON_EDITOR QIcon( QString( ":/res/icon/editor.png" ) )
#define ICON_INTERNET QIcon( QString( ":/res/icon/internet.png" ) )

#define ICON_PLUS QIcon( QString( ":/res/icon/plus.png" ) )
#define ICON_MINUS QIcon( QString( ":/res/icon/minus.png" ) )
#endif
