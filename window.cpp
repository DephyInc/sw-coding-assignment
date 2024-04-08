#include <QGridLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <iostream>
#include <QDialog>
#include <QFont>
#include "window.h"
#include "vEPROM_cmds.h"

Window::Window() {
    createGridGroupBox();

    guiOutText = new QTextEdit;
    QFont font("Courier", 10);
    guiOutText->setFont(font);
    //guiOutText->setPlainText(tr("Output will be here."));

    QVBoxLayout *mainLayout = new QVBoxLayout;

    mainLayout->addWidget(gridGroupBox);
    mainLayout->addWidget(guiOutText);

    setLayout(mainLayout);
    resize(690,900);
    setWindowTitle(tr("vEPROM Window"));
#ifdef GUI_VEPROM
    vEPROM_cmds_wo = new vEPROM_cmds(guiOutText);
#else
    vEPROM_cmds_wo = new vEPROM_cmds;
#endif
}

Window::~Window() {
    if (gridGroupBox != nullptr)
        delete gridGroupBox;
    if (guiOutText != nullptr)
        delete guiOutText;
    if (buttonBox != nullptr)
        delete buttonBox;

    if (exitAction != nullptr)
        delete exitAction;

    // 0) for titile "Command     Parameters     Run Button"
    if (cmd_label != nullptr)
        delete cmd_label;
    if (para_label != nullptr)
        delete para_label;
    if (run_label != nullptr)
        delete run_label;

    // 1) for veprom create capacity
    if (create_label != nullptr)
        delete create_label;
    if (create_args != nullptr)
        delete create_args;
    if (create_button != nullptr)
        delete create_button;

    // 2) for veprom load file
    if (load_label != nullptr)
        delete load_label;
    if (load_args != nullptr)
        delete load_args;
    if (load_button != nullptr)
        delete load_button;

    // 3) for veprom write_raw address string
    if (write_raw_label != nullptr)
        delete write_raw_label;
    if (write_raw_args != nullptr)
        delete write_raw_args;
    if (write_raw_button != nullptr)
        delete write_raw_button;

    // 4) for veprom read_raw address length
    if (read_raw_label != nullptr)
        delete read_raw_label;
    if (read_raw_args != nullptr)
        delete read_raw_args;
    if (read_raw_button != nullptr)
        delete read_raw_button;

    // 5) for veprom write file
    if (write_label != nullptr)
        delete write_label;
    if (write_args != nullptr)
        delete write_args;
    if (write_button != nullptr)
        delete write_button;

    // 6) for veprom list
    if (list_label != nullptr)
        delete list_label;
    if (listPara != nullptr)
        delete listPara;
    if (list_button != nullptr)
        delete list_button;

    // 7) for veprom read file
    if (read_label != nullptr)
        delete read_label;
    if (read_args != nullptr)
        delete read_args;
    if (read_button != nullptr)
        delete read_button;

    // 8) for veprom erase
    if (erase_label != nullptr)
        delete erase_label;
    if (erasePara != nullptr)
        delete erasePara;
    if (erase_button != nullptr)
        delete erase_button;

    // 9) for Quit/Exit
    if (quit_button != nullptr)
        delete quit_button;

    if (vEPROM_cmds_wo != nullptr)
        delete vEPROM_cmds_wo;
}

void Window::createGridGroupBox() {
    gridGroupBox = new QGroupBox(tr("veprom"));
    QGridLayout *layout = new QGridLayout;

    unsigned int i = 1;
    // 0) for titile "Command     Parameters     Run Button"
    cmd_label = new QLabel(tr("                Command"));
    para_label = new QLabel(tr("                Parameters"));
    run_label = new QLabel(tr("Run Button"));
    layout->addWidget(cmd_label, i, 0);
    layout->addWidget(para_label, i, 1);
    layout->addWidget(run_label, i, 2);
    i++;

    // 1) for veprom create capacity
    create_label = new QLabel(tr("                create"));
    create_args = new QLineEdit;
    create_args->setPlaceholderText("input capacity here");
    create_button = new QPushButton("Go Create");
    create_button->setEnabled(false);
    connect(create_args, &QLineEdit::textChanged,
            this, &Window::enable_create_button);
    connect(create_button, &QPushButton::clicked,
            this, &Window::get_create_args);
    layout->addWidget(create_label, i, 0);
    layout->addWidget(create_args, i, 1);
    layout->addWidget(create_button, i, 2);
    i++;

    // 2) for veprom load file
    load_label = new QLabel(tr("                load"));
    load_args = new QLineEdit;
    load_args->setPlaceholderText("input file name here");
    load_button = new QPushButton(tr("Go Load"));
    load_button->setEnabled(false);
    connect(load_args, &QLineEdit::textChanged,
            this, &Window::enable_load_button);
    connect(load_button, &QPushButton::clicked,
            this, &Window::get_load_args);
    layout->addWidget(load_label, i, 0);
    layout->addWidget(load_args, i, 1);
    layout->addWidget(load_button, i, 2);

    i++;

    // 3) for veprom write_raw address string
    write_raw_label = new QLabel(tr("                write_raw"));
    write_raw_args = new QLineEdit;
    write_raw_args->setPlaceholderText("input address and string here");
    write_raw_button = new QPushButton(tr("Go Write_raw"));
    write_raw_button->setEnabled(false);
    connect(write_raw_args, &QLineEdit::textChanged,
            this, &Window::enable_write_raw_button);
    connect(write_raw_button, &QPushButton::clicked,
            this, &Window::get_write_raw_args);
    layout->addWidget(write_raw_label, i, 0);
    layout->addWidget(write_raw_args, i, 1);
    layout->addWidget(write_raw_button, i, 2);
    i++;

    // 4) for veprom read_raw address length
    read_raw_label = new QLabel(tr("                read_raw"));
    read_raw_args = new QLineEdit;
    read_raw_args->setPlaceholderText("input address and length here");
    read_raw_button = new QPushButton(tr("Go Read_raw"));
    read_raw_button->setEnabled(false);
    connect(read_raw_args, &QLineEdit::textChanged,
            this, &Window::enable_read_raw_button);
    connect(read_raw_button, &QPushButton::clicked,
            this, &Window::get_read_raw_args);
    layout->addWidget(read_raw_label, i, 0);
    layout->addWidget(read_raw_args, i, 1);
    layout->addWidget(read_raw_button, i, 2);
    i++;

    // 5) for veprom write file
    write_label = new QLabel(tr("                write"));
    write_args = new QLineEdit;
    write_args->setPlaceholderText("input file name here");
    write_button = new QPushButton(tr("Go Write"));
    write_button->setEnabled(false);
    connect(write_args, &QLineEdit::textChanged,
            this, &Window::enable_write_button);
    connect(write_button, &QPushButton::clicked,
            this, &Window::get_write_args);
    layout->addWidget(write_label, i, 0);
    layout->addWidget(write_args, i, 1);
    layout->addWidget(write_button, i, 2);
    i++;

    // 6) for veprom list
    listPara = new QLabel(tr(" lists files for EPROMs and files in current vEPROM -->"));
    list_label = new QLabel(tr("                list"));
    list_button = new QPushButton(tr("Go List"));
    connect(list_button, &QPushButton::clicked,
            this, &Window::get_list_args);
    layout->addWidget(list_label, i, 0);
    layout->addWidget(listPara, i, 1);
    layout->addWidget(list_button, i, 2);
    i++;

    // 7) for veprom read file
    read_label = new QLabel(tr("                read"));
    read_args = new QLineEdit;
    read_args->setPlaceholderText("input file name here");
    read_button = new QPushButton(tr("Go Read"));
    read_button->setEnabled(false);
    connect(read_args, &QLineEdit::textChanged,
            this, &Window::enable_read_button);
    connect(read_button, &QPushButton::clicked,
            this, &Window::get_read_args);
    layout->addWidget(read_label, i, 0);
    layout->addWidget(read_args, i, 1);
    layout->addWidget(read_button, i, 2);
    i++;

    // 8) for veprom erase
    erasePara = new QLabel(tr(" sets EPROM back to its original state -->"));
    erase_label = new QLabel(tr("                erase"));
    erase_button = new QPushButton(tr("Go Erase"));
    connect(erase_button, &QPushButton::clicked,
            this, &Window::get_erase_args);
    layout->addWidget(erase_label, i, 0);
    layout->addWidget(erasePara, i, 1);
    layout->addWidget(erase_button, i, 2);
    i++;

    // 9) for Quit/Exit
    QLabel t9(tr("                "));
    QLabel ta(tr("                "));
    quit_button = new QPushButton(tr("Quit"));
    connect(quit_button, &QPushButton::clicked,
            this, &Window::quit_handle);
    layout->addWidget(&t9, i, 0);
    layout->addWidget(&ta, i, 1);
    layout->addWidget(quit_button, i, 2);

    gridGroupBox->setLayout(layout);
}

void Window::get_create_args() {
    QString text = create_args->text();
    std::string str = text.toStdString();
    args.clear();
    vEPROM_cmds_wo->get_tokens(args, str, delimiter);
    vEPROM_cmds_wo->create_cmd(args);
    std::cout << std::endl;
    create_button->setEnabled(false);
}

void Window::get_load_args() {
    QString text = load_args->text();
    std::string str = text.toStdString();

    args.clear();
    vEPROM_cmds_wo->get_tokens(args, str, delimiter);
    vEPROM_cmds_wo->load_cmd(args);
    std::cout << std::endl;
    load_button->setEnabled(false);
}

void Window::get_write_raw_args() {
    QString text = write_raw_args->text();
    std::string str = text.toStdString();

    args.clear();
    vEPROM_cmds_wo->get_tokens(args, str, delimiter);
    vEPROM_cmds_wo->write_raw_cmd(args);
    std::cout << std::endl;
    write_raw_button->setEnabled(false);
}

void Window::get_read_raw_args() {
    QString text = read_raw_args->text();
    std::string str = text.toStdString();

    args.clear();
    vEPROM_cmds_wo->get_tokens(args, str, delimiter);
    vEPROM_cmds_wo->read_raw_cmd(args);
    std::cout << std::endl;
    read_raw_button->setEnabled(false);
}

void Window::get_write_args() {
    QString text = write_args->text();
    std::string str = text.toStdString();

    args.clear();
    vEPROM_cmds_wo->get_tokens(args, str, delimiter);
    vEPROM_cmds_wo->write_cmd(args);
    std::cout << std::endl;
    write_button->setEnabled(false);
}

void Window::get_list_args() {
    vEPROM_cmds_wo->list_cmd();
    std::cout << std::endl;
}

void Window::get_read_args() {
    QString text = read_args->text();
    std::string str = text.toStdString();

    args.clear();
    vEPROM_cmds_wo->get_tokens(args, str, delimiter);
    vEPROM_cmds_wo->read_cmd(args);
    std::cout << std::endl;
    read_button->setEnabled(false);
}

void Window::get_erase_args() {
    vEPROM_cmds_wo->erase_cmd();
    std::cout << std::endl;
}

void Window::quit_handle() {
    std::exit(EXIT_SUCCESS);
}

void Window::enable_create_button() {
    create_button->setEnabled(true);
}

void Window::enable_load_button() {
    load_button->setEnabled(true);
}

void Window::enable_write_raw_button() {
    write_raw_button->setEnabled(true);
}

void Window::enable_read_raw_button() {
    read_raw_button->setEnabled(true);
}

void Window::enable_write_button() {
    write_button->setEnabled(true);
}

void Window::enable_read_button() {
    read_button->setEnabled(true);
}
