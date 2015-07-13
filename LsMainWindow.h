#ifndef LSMAINWINDOW_H
#define LSMAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class LsMainWindow;
}

class LsMainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit LsMainWindow(QWidget *parent = 0);
    ~LsMainWindow();
    
private:
    Ui::LsMainWindow *ui;
};

#endif // LSMAINWINDOW_H
