/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 6.9.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QVBoxLayout *verticalLayout_main;
    QTabWidget *tabWidget;
    QWidget *tab_requetes;
    QVBoxLayout *verticalLayout_req;
    QHBoxLayout *horizontalLayout_send;
    QComboBox *combo_requetes;
    QPushButton *btn_envoyer;
    QTableWidget *table_requetes;
    QWidget *tab_valeurs;
    QGridLayout *gridLayout_valeurs;
    QGroupBox *group_temp;
    QVBoxLayout *vboxLayout;
    QLabel *lbl_val_temp;
    QGroupBox *group_pression;
    QVBoxLayout *vboxLayout1;
    QLabel *lbl_val_pression;
    QGroupBox *group_alt;
    QVBoxLayout *vboxLayout2;
    QLabel *lbl_val_alt;
    QGroupBox *group_rssi;
    QVBoxLayout *vboxLayout3;
    QLabel *lbl_val_rssi;
    QGroupBox *group_snr;
    QVBoxLayout *vboxLayout4;
    QLabel *lbl_val_snr;
    QWidget *tab_images;
    QVBoxLayout *verticalLayout_img;
    QGroupBox *group_last_img;
    QVBoxLayout *verticalLayout_img_main;
    QLabel *lbl_image_principale;
    QGroupBox *group_prev_imgs;
    QHBoxLayout *horizontalLayout_prev_img;
    QLabel *lbl_img_prev1;
    QLabel *lbl_img_prev2;
    QLabel *lbl_img_prev3;
    QPushButton *btn_generer_video;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName("Widget");
        Widget->resize(850, 650);
        verticalLayout_main = new QVBoxLayout(Widget);
        verticalLayout_main->setObjectName("verticalLayout_main");
        tabWidget = new QTabWidget(Widget);
        tabWidget->setObjectName("tabWidget");
        tab_requetes = new QWidget();
        tab_requetes->setObjectName("tab_requetes");
        verticalLayout_req = new QVBoxLayout(tab_requetes);
        verticalLayout_req->setObjectName("verticalLayout_req");
        horizontalLayout_send = new QHBoxLayout();
        horizontalLayout_send->setObjectName("horizontalLayout_send");
        combo_requetes = new QComboBox(tab_requetes);
        combo_requetes->addItem(QString());
        combo_requetes->setObjectName("combo_requetes");
        QSizePolicy sizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(combo_requetes->sizePolicy().hasHeightForWidth());
        combo_requetes->setSizePolicy(sizePolicy);

        horizontalLayout_send->addWidget(combo_requetes);

        btn_envoyer = new QPushButton(tab_requetes);
        btn_envoyer->setObjectName("btn_envoyer");

        horizontalLayout_send->addWidget(btn_envoyer);


        verticalLayout_req->addLayout(horizontalLayout_send);

        table_requetes = new QTableWidget(tab_requetes);
        if (table_requetes->columnCount() < 3)
            table_requetes->setColumnCount(3);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        table_requetes->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        table_requetes->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        QTableWidgetItem *__qtablewidgetitem2 = new QTableWidgetItem();
        table_requetes->setHorizontalHeaderItem(2, __qtablewidgetitem2);
        table_requetes->setObjectName("table_requetes");
        table_requetes->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table_requetes->setSelectionBehavior(QAbstractItemView::SelectRows);

        verticalLayout_req->addWidget(table_requetes);

        tabWidget->addTab(tab_requetes, QString());
        tab_valeurs = new QWidget();
        tab_valeurs->setObjectName("tab_valeurs");
        gridLayout_valeurs = new QGridLayout(tab_valeurs);
        gridLayout_valeurs->setObjectName("gridLayout_valeurs");
        group_temp = new QGroupBox(tab_valeurs);
        group_temp->setObjectName("group_temp");
        vboxLayout = new QVBoxLayout(group_temp);
        vboxLayout->setObjectName("vboxLayout");
        lbl_val_temp = new QLabel(group_temp);
        lbl_val_temp->setObjectName("lbl_val_temp");
        QFont font;
        font.setFamilies({QString::fromUtf8("Consolas")});
        font.setPointSize(20);
        font.setBold(true);
        lbl_val_temp->setFont(font);
        lbl_val_temp->setAlignment(Qt::AlignCenter);

        vboxLayout->addWidget(lbl_val_temp);


        gridLayout_valeurs->addWidget(group_temp, 0, 0, 1, 1);

        group_pression = new QGroupBox(tab_valeurs);
        group_pression->setObjectName("group_pression");
        vboxLayout1 = new QVBoxLayout(group_pression);
        vboxLayout1->setObjectName("vboxLayout1");
        lbl_val_pression = new QLabel(group_pression);
        lbl_val_pression->setObjectName("lbl_val_pression");
        lbl_val_pression->setFont(font);
        lbl_val_pression->setAlignment(Qt::AlignCenter);

        vboxLayout1->addWidget(lbl_val_pression);


        gridLayout_valeurs->addWidget(group_pression, 0, 1, 1, 1);

        group_alt = new QGroupBox(tab_valeurs);
        group_alt->setObjectName("group_alt");
        vboxLayout2 = new QVBoxLayout(group_alt);
        vboxLayout2->setObjectName("vboxLayout2");
        lbl_val_alt = new QLabel(group_alt);
        lbl_val_alt->setObjectName("lbl_val_alt");
        lbl_val_alt->setFont(font);
        lbl_val_alt->setAlignment(Qt::AlignCenter);

        vboxLayout2->addWidget(lbl_val_alt);


        gridLayout_valeurs->addWidget(group_alt, 1, 0, 1, 1);

        group_rssi = new QGroupBox(tab_valeurs);
        group_rssi->setObjectName("group_rssi");
        vboxLayout3 = new QVBoxLayout(group_rssi);
        vboxLayout3->setObjectName("vboxLayout3");
        lbl_val_rssi = new QLabel(group_rssi);
        lbl_val_rssi->setObjectName("lbl_val_rssi");
        lbl_val_rssi->setFont(font);
        lbl_val_rssi->setAlignment(Qt::AlignCenter);

        vboxLayout3->addWidget(lbl_val_rssi);


        gridLayout_valeurs->addWidget(group_rssi, 1, 1, 1, 1);

        group_snr = new QGroupBox(tab_valeurs);
        group_snr->setObjectName("group_snr");
        vboxLayout4 = new QVBoxLayout(group_snr);
        vboxLayout4->setObjectName("vboxLayout4");
        lbl_val_snr = new QLabel(group_snr);
        lbl_val_snr->setObjectName("lbl_val_snr");
        lbl_val_snr->setFont(font);
        lbl_val_snr->setAlignment(Qt::AlignCenter);

        vboxLayout4->addWidget(lbl_val_snr);


        gridLayout_valeurs->addWidget(group_snr, 2, 0, 1, 2);

        tabWidget->addTab(tab_valeurs, QString());
        tab_images = new QWidget();
        tab_images->setObjectName("tab_images");
        verticalLayout_img = new QVBoxLayout(tab_images);
        verticalLayout_img->setObjectName("verticalLayout_img");
        group_last_img = new QGroupBox(tab_images);
        group_last_img->setObjectName("group_last_img");
        verticalLayout_img_main = new QVBoxLayout(group_last_img);
        verticalLayout_img_main->setObjectName("verticalLayout_img_main");
        lbl_image_principale = new QLabel(group_last_img);
        lbl_image_principale->setObjectName("lbl_image_principale");
        QSizePolicy sizePolicy1(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(lbl_image_principale->sizePolicy().hasHeightForWidth());
        lbl_image_principale->setSizePolicy(sizePolicy1);
        lbl_image_principale->setFrameShape(QFrame::Box);
        lbl_image_principale->setAlignment(Qt::AlignCenter);

        verticalLayout_img_main->addWidget(lbl_image_principale);


        verticalLayout_img->addWidget(group_last_img);

        group_prev_imgs = new QGroupBox(tab_images);
        group_prev_imgs->setObjectName("group_prev_imgs");
        group_prev_imgs->setMaximumSize(QSize(16777215, 150));
        horizontalLayout_prev_img = new QHBoxLayout(group_prev_imgs);
        horizontalLayout_prev_img->setObjectName("horizontalLayout_prev_img");
        lbl_img_prev1 = new QLabel(group_prev_imgs);
        lbl_img_prev1->setObjectName("lbl_img_prev1");
        lbl_img_prev1->setFrameShape(QFrame::Box);
        lbl_img_prev1->setAlignment(Qt::AlignCenter);

        horizontalLayout_prev_img->addWidget(lbl_img_prev1);

        lbl_img_prev2 = new QLabel(group_prev_imgs);
        lbl_img_prev2->setObjectName("lbl_img_prev2");
        lbl_img_prev2->setFrameShape(QFrame::Box);
        lbl_img_prev2->setAlignment(Qt::AlignCenter);

        horizontalLayout_prev_img->addWidget(lbl_img_prev2);

        lbl_img_prev3 = new QLabel(group_prev_imgs);
        lbl_img_prev3->setObjectName("lbl_img_prev3");
        lbl_img_prev3->setFrameShape(QFrame::Box);
        lbl_img_prev3->setAlignment(Qt::AlignCenter);

        horizontalLayout_prev_img->addWidget(lbl_img_prev3);


        verticalLayout_img->addWidget(group_prev_imgs);

        btn_generer_video = new QPushButton(tab_images);
        btn_generer_video->setObjectName("btn_generer_video");
        btn_generer_video->setMinimumSize(QSize(0, 40));

        verticalLayout_img->addWidget(btn_generer_video);

        tabWidget->addTab(tab_images, QString());

        verticalLayout_main->addWidget(tabWidget);


        retranslateUi(Widget);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Mission Control - Nacelle Alpha", nullptr));
        Widget->setStyleSheet(QCoreApplication::translate("Widget", "\n"
"    /* Le fond : Espace profond */\n"
"    QWidget {\n"
"        background-color: #05080f;\n"
"        color: #00ffcc; /* Cyan N\303\251on pour la t\303\251l\303\251m\303\251trie */\n"
"        font-family: 'Consolas', 'Courier New', monospace; /* Look console informatique */\n"
"    }\n"
"\n"
"    /* Le conteneur principal */\n"
"    QTabWidget::pane {\n"
"        border: 1px solid #005577;\n"
"        background: #0b0f19;\n"
"    }\n"
"\n"
"    /* Les onglets : \303\211crans de contr\303\264le inactifs */\n"
"    QTabBar::tab {\n"
"        background: #05080f;\n"
"        color: #0088aa;\n"
"        padding: 8px 25px;\n"
"        border: 1px solid #005577;\n"
"        border-bottom: none;\n"
"        border-top-left-radius: 4px;\n"
"        border-top-right-radius: 4px;\n"
"        margin-right: 2px;\n"
"    }\n"
"\n"
"    /* L'onglet actif : \303\211cran allum\303\251 */\n"
"    QTabBar::tab:selected {\n"
"        background: #0b0f19;\n"
"        color: #00ffcc;\n"
"        border: 1px solid #00ffcc;\n"
""
                        "        border-bottom: none;\n"
"        font-weight: bold;\n"
"    }\n"
"\n"
"    /* Les cadres de donn\303\251es (Temp\303\251rature, etc.) : Interface HUD */\n"
"    QGroupBox {\n"
"        border: 1px solid #0088aa;\n"
"        border-radius: 4px;\n"
"        margin-top: 15px;\n"
"        background-color: rgba(11, 15, 25, 0.8);\n"
"    }\n"
"    QGroupBox::title {\n"
"        subcontrol-origin: margin;\n"
"        left: 10px;\n"
"        padding: 0 5px;\n"
"        color: #ffffff;\n"
"        font-weight: bold;\n"
"    }\n"
"\n"
"    /* Les labels (Les valeurs elles-m\303\252mes) */\n"
"    QLabel {\n"
"        color: #ffffff; /* Valeurs en blanc pour contraster avec le cyan */\n"
"    }\n"
"\n"
"    /* Les boutons : Panneaux d'activation */\n"
"    QPushButton {\n"
"        background-color: #003344;\n"
"        color: #00ffcc;\n"
"        border: 1px solid #0088aa;\n"
"        border-radius: 3px;\n"
"        padding: 8px 15px;\n"
"        font-weight: bold;\n"
"        text-transform: uppercase;\n"
"   "
                        " }\n"
"    QPushButton:hover {\n"
"        background-color: #005577;\n"
"        border: 1px solid #00ffcc;\n"
"        color: #ffffff;\n"
"    }\n"
"\n"
"    /* Bouton d'action principale (Vid\303\251o) : Surcharge propulseur */\n"
"    QPushButton#btn_generer_video {\n"
"        background-color: #4a0011;\n"
"        color: #ff3366;\n"
"        border: 1px solid #aa0033;\n"
"    }\n"
"    QPushButton#btn_generer_video:hover {\n"
"        background-color: #770022;\n"
"        color: #ffffff;\n"
"        border: 1px solid #ff3366;\n"
"    }\n"
"\n"
"    /* Le tableau des requ\303\252tes : Journal de bord */\n"
"    QTableWidget {\n"
"        background-color: #020408;\n"
"        gridline-color: #004466;\n"
"        color: #aaddff;\n"
"        border: 1px solid #005577;\n"
"        selection-background-color: #005577;\n"
"        selection-color: #ffffff;\n"
"    }\n"
"    QHeaderView::section {\n"
"        background-color: #002233;\n"
"        color: #00ffcc;\n"
"        font-weight: bold;\n"
"        bord"
                        "er: 1px solid #004466;\n"
"        padding: 4px;\n"
"    }\n"
"    \n"
"    /* Combobox (Menu d\303\251roulant des requ\303\252tes) */\n"
"    QComboBox {\n"
"        background-color: #003344;\n"
"        color: #00ffcc;\n"
"        border: 1px solid #0088aa;\n"
"        padding: 5px;\n"
"    }\n"
"   ", nullptr));
        combo_requetes->setItemText(0, QCoreApplication::translate("Widget", "R\303\211CUP\303\211RER RSSI & SNR", nullptr));

        btn_envoyer->setText(QCoreApplication::translate("Widget", "\303\211METTRE REQU\303\212TE", nullptr));
        QTableWidgetItem *___qtablewidgetitem = table_requetes->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("Widget", "HORODATAGE", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = table_requetes->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("Widget", "REQU\303\212TE / R\303\211PONSE", nullptr));
        QTableWidgetItem *___qtablewidgetitem2 = table_requetes->horizontalHeaderItem(2);
        ___qtablewidgetitem2->setText(QCoreApplication::translate("Widget", "LATENCE (ms)", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_requetes), QCoreApplication::translate("Widget", "TRANSMISSIONS", nullptr));
        group_temp->setTitle(QCoreApplication::translate("Widget", "TEMP\303\211RATURE INT\303\211RIEURE", nullptr));
        lbl_val_temp->setText(QCoreApplication::translate("Widget", "-- \302\260C", nullptr));
        group_pression->setTitle(QCoreApplication::translate("Widget", "PRESSION ATMOSPH\303\211RIQUE", nullptr));
        lbl_val_pression->setText(QCoreApplication::translate("Widget", "-- hPa", nullptr));
        group_alt->setTitle(QCoreApplication::translate("Widget", "ALTITUDE RELATIVE", nullptr));
        lbl_val_alt->setText(QCoreApplication::translate("Widget", "-- m", nullptr));
        group_rssi->setTitle(QCoreApplication::translate("Widget", "SIGNAL RSSI", nullptr));
        lbl_val_rssi->setText(QCoreApplication::translate("Widget", "-- dBm", nullptr));
        group_snr->setTitle(QCoreApplication::translate("Widget", "RAPPORT SIGNAL/BRUIT (SNR)", nullptr));
        lbl_val_snr->setText(QCoreApplication::translate("Widget", "-- dB", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_valeurs), QCoreApplication::translate("Widget", "T\303\211L\303\211M\303\211TRIE CAPTEURS", nullptr));
        group_last_img->setTitle(QCoreApplication::translate("Widget", "DERNI\303\210RE CAPTURE [ EN ATTENTE DU SIGNAL... ]", nullptr));
        lbl_image_principale->setText(QCoreApplication::translate("Widget", "[ AUCUN SIGNAL VID\303\211O D\303\211TECT\303\211 ]", nullptr));
        group_prev_imgs->setTitle(QCoreApplication::translate("Widget", "CACHE TAMPON (HISTORIQUE R\303\211CENT)", nullptr));
        lbl_img_prev1->setText(QCoreApplication::translate("Widget", "IMG T-1", nullptr));
        lbl_img_prev2->setText(QCoreApplication::translate("Widget", "IMG T-2", nullptr));
        lbl_img_prev3->setText(QCoreApplication::translate("Widget", "IMG T-3", nullptr));
        btn_generer_video->setText(QCoreApplication::translate("Widget", "COMPILER S\303\211QUENCE VID\303\211O", nullptr));
        tabWidget->setTabText(tabWidget->indexOf(tab_images), QCoreApplication::translate("Widget", "FLUX OPTIQUE", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
