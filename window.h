#ifndef WINDOW_H
#define WINDOW_H

#include <QMainWindow>
#include <QDialog>
#include <QMenuBar>
#include <QGroupBox>
#include <QTextEdit>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QDialog>
#include "vEPROM_cmds.h"

class Window : public QDialog
{
    Q_OBJECT

public:
    Window();
    ~Window();

    void createGridGroupBox();

    void get_create_args();
    void get_load_args();
    void get_write_raw_args();
    void get_read_raw_args();
    void get_write_args();
    void get_list_args();
    void get_read_args();
    void get_erase_args();
    void quit_handle();

    void enable_create_button();
    void enable_load_button();
    void enable_write_raw_button();
    void enable_read_raw_button();
    void enable_write_button();
    void enable_read_button();

    QGroupBox *gridGroupBox;
    QTextEdit *guiOutText;
    QDialogButtonBox *buttonBox;

    QAction *exitAction;

    // 0) for titile "Command     Parameters     Run Button"
    QLabel *cmd_label;
    QLabel *para_label;
    QLabel *run_label;

    // 1) for veprom create capacity
    QLabel *create_label;
    QLineEdit *create_args;
    QPushButton *create_button;

    // 2) for veprom load file
    QLabel *load_label;
    QLineEdit *load_args;
    QPushButton *load_button;

    // 3) for veprom write_raw address string
    QLabel *write_raw_label;
    QLineEdit *write_raw_args;
    QPushButton *write_raw_button;

    // 4) for veprom read_raw address length
    QLabel *read_raw_label;
    QLineEdit *read_raw_args;
    QPushButton *read_raw_button;

    // 5) for veprom write file
    QLabel *write_label;
    QLineEdit *write_args;
    QPushButton *write_button;

    // 6) for veprom list
    QLabel *list_label;
    QLabel *listPara;
    QPushButton *list_button;

    // 7) for veprom read file
    QLabel *read_label;
    QLineEdit *read_args;
    QPushButton *read_button;

    // 8) for veprom erase
    QLabel *erase_label;
    QLabel *erasePara;
    QPushButton *erase_button;

    // 9) for Quit/Exit
    QPushButton *quit_button;

    std::vector<std::string> args;
    char delimiter = ' ';
    vEPROM_cmds *vEPROM_cmds_wo;
};
#endif // WINDOW_H
