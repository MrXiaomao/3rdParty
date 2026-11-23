#include "qgoodwindowhelper.h"

QGoodWindowHelper::QGoodWindowHelper(QWidget *parent)
    : QGoodWindow(parent) {

}

QGoodWindowHelper::QGoodWindowHelper(QMainWindow *centralWidget, QWidget *parent)
    : QGoodWindow(parent)
{
    centralWidget->setParent(this);
    this->setupUiHelper(centralWidget);

    centralWidget->installEventFilter(this);

    // 主窗口创建子窗口时添加属性
    centralWidget->setAttribute(Qt::WA_DeleteOnClose);
}

void QGoodWindowHelper::setupUiHelper(QMainWindow* centralWidget)
{
    this->setupUiHelper(centralWidget, QGoodWindow::isSystemThemeDark());
}

void QGoodWindowHelper::setupUiHelper(QMainWindow* centralWidget, bool isDarkTheme)
{
    mCentralWidget = centralWidget;
    mCentralWidget->setWindowFlags(Qt::Widget);
    mGoodCentraWidget = new QGoodCentralWidget(this);

    connect(qGoodStateHolder, &QGoodStateHolder::currentThemeChanged, this, [](){
        if (qGoodStateHolder->isCurrentThemeDark())
            QGoodWindow::setAppDarkTheme();
        else
            QGoodWindow::setAppLightTheme();
    });
    connect(this, &QGoodWindow::systemThemeChanged, this, [&]{
        qGoodStateHolder->setCurrentThemeDark(QGoodWindow::isSystemThemeDark());
    });

#ifdef Q_OS_MAC
    //macOS uses global menu bar
    if(QApplication::testAttribute(Qt::AA_DontUseNativeMenuBar)) {
#else
    if(true) {
#endif
        mMenuBar = mCentralWidget->menuBar();
        if (mMenuBar)
        {
            //Set font of menu bar
            QFont font = mMenuBar->font();
#ifdef Q_OS_WIN
            font.setFamily("Segoe UI");
#else
            font.setFamily(qApp->font().family());
#endif
            //mMenuBar->setFont(font);

            QTimer::singleShot(0, this, [&]{
                const int title_bar_height = mGoodCentraWidget->titleBarHeight();
                mMenuBar->setStyleSheet(QString("QMenuBar {height: %0px;}").arg(title_bar_height));
            });

            connect(mGoodCentraWidget,&QGoodCentralWidget::windowActiveChanged,this, [&](bool active){
                mMenuBar->setEnabled(active);
            });

            // 给菜单添加主题切换子菜单项
            // 遍历菜单栏所有Action，判断主题切换菜单是否存在
            bool themeMenuExist = false;
            for (QAction* act : mMenuBar->actions()) {
                // 仅判断带菜单对象、且文本完全匹配的项
                if (act->menu() && act->menu()->objectName() == QString::fromUtf8("menu_theme")) {
                    themeMenuExist = true;
                    break;
                }
            }

            if (!themeMenuExist){
                QAction* action_lightTheme = new QAction(mCentralWidget);
                action_lightTheme->setObjectName(QString::fromUtf8("action_lightTheme"));
                action_lightTheme->setCheckable(true);
                action_lightTheme->setText(tr("浅色"));
                QAction* action_darkTheme = new QAction(mCentralWidget);
                action_darkTheme->setObjectName(QString::fromUtf8("action_darkTheme"));
                action_darkTheme->setCheckable(true);
                action_darkTheme->setText(tr("深色"));
                QAction* action_colorTheme = new QAction(mCentralWidget);
                action_colorTheme->setObjectName(QString::fromUtf8("action_colorTheme"));
                action_colorTheme->setText(tr("主题颜色"));

                QMenu* menu_theme = new QMenu(mMenuBar);
                menu_theme->setObjectName(QString::fromUtf8("menu_theme"));
                menu_theme->setTitle(tr("主题"));
                menu_theme->addAction(action_lightTheme);
                menu_theme->addAction(action_darkTheme);
                menu_theme->addAction(action_colorTheme);

                // 取倒数第2个菜单项作为插入位置锚点
                QList<QAction*> menuActions = mMenuBar->actions();
                if (menuActions.size() < 2)
                    mMenuBar->addAction(menu_theme->menuAction());
                else{
                    // 目标插入位置：倒数第2个菜单项位置，取对应位置的Action作为插入锚点
                    QAction* anchorAction = menuActions[menuActions.size() - 1];
                    mMenuBar->insertMenu(anchorAction, menu_theme);
                }

                connect(action_lightTheme, &QAction::triggered, this, [=]{
                    bool darkTheme = false;
                    QSettings settings;
                    QColor themeColor = settings.value("Global/MainWindow/lightThemeColor", QColor(30,30,30)).value<QColor>();

                    qGoodStateHolder->setCurrentThemeDark(darkTheme);
                    QGoodWindow::setAppCustomTheme(darkTheme, themeColor);
                    settings.setValue("Global/MainWindow/darkTheme", darkTheme);
                });
                connect(action_darkTheme, &QAction::triggered, this, [=]{
                    bool darkTheme = true;
                    QSettings settings;
                    QColor themeColor = settings.value("Global/MainWindow/darkThemeColor", QColor(30,30,30)).value<QColor>();

                    qGoodStateHolder->setCurrentThemeDark(darkTheme);
                    QGoodWindow::setAppCustomTheme(darkTheme, themeColor);
                    settings.setValue("Global/MainWindow/darkTheme", darkTheme);
                });
                connect(action_colorTheme, &QAction::triggered, this, [=]{
                    QSettings settings;
                    bool darkTheme = settings.value("Global/MainWindow/darkTheme", isDarkTheme).toBool();
                    QColor themeColor = settings.value("Global/MainWindow/lightThemeColor", QColor(30,30,30)).value<QColor>();
                    if (darkTheme)
                        themeColor = settings.value("Global/MainWindow/darkThemeColor", QColor(30,30,30)).value<QColor>();
                    themeColor = QColorDialog::getColor(themeColor, this, tr("选择颜色"));
                    if (themeColor.isValid()) {
                        qGoodStateHolder->setCurrentThemeDark(darkTheme);
                        QGoodWindow::setAppCustomTheme(darkTheme,themeColor);

                        if (darkTheme)
                            settings.setValue("Global/MainWindow/darkThemeColor", themeColor);
                        else
                            settings.setValue("Global/MainWindow/lightThemeColor", themeColor);
                    }
                });

                QSettings settings;
                bool darkTheme = settings.value("Global/MainWindow/darkTheme", isDarkTheme).toBool();

                QActionGroup *themeActionGroup = new QActionGroup(this);
                action_lightTheme->setActionGroup(themeActionGroup);
                action_darkTheme->setActionGroup(themeActionGroup);
                action_darkTheme->setChecked(darkTheme);
                action_lightTheme->setChecked(!darkTheme);
            }

            mGoodCentraWidget->setLeftTitleBarWidget(mMenuBar);
        }
    }

    QSettings settings;
    bool darkTheme = settings.value("Global/MainWindow/darkTheme", isDarkTheme).toBool();
    QColor themeColor = settings.value("Global/MainWindow/lightThemeColor", QColor(30,30,30)).value<QColor>();
    if (isDarkTheme)
        themeColor = settings.value("Global/MainWindow/darkThemeColor", QColor(30,30,30)).value<QColor>();
    qGoodStateHolder->setCurrentThemeDark(darkTheme);
    QGoodWindow::setAppCustomTheme(darkTheme, themeColor);

    mGoodCentraWidget->setCentralWidget(mCentralWidget);
    setCentralWidget(mGoodCentraWidget);
    fixMenuBarWidth();

    if (!mCentralWidget->windowIcon().isNull())
        setWindowIcon(mCentralWidget->windowIcon());
    setWindowTitle(mCentralWidget->windowTitle());

    mGoodCentraWidget->setTitleAlignment(/*Qt::AlignVCenter | Qt::AlignLeft*/Qt::AlignCenter);
}

QGoodWindowHelper::~QGoodWindowHelper()
{
    delete mCentralWidget;
}

void QGoodWindowHelper::closeEvent(QCloseEvent *event)
{
    if (mCentralWidget)
        QApplication::sendEvent(mCentralWidget, event);
}

bool QGoodWindowHelper::event(QEvent * event)
{
    if(event->type() == QEvent::StatusTip) {
        return QApplication::sendEvent(mCentralWidget, event);
    }

    return QGoodWindow::event(event);
}

bool QGoodWindowHelper::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::Destroy && watched == mCentralWidget){
        mCentralWidget = nullptr;
        this->close();
        return true;
    }

    return QMainWindow::eventFilter(watched, event);
}
