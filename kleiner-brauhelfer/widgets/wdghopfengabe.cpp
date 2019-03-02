#include "wdghopfengabe.h"
#include "ui_wdghopfengabe.h"
#include <QStandardItemModel>
#include "brauhelfer.h"
#include "settings.h"
#include "model/rohstoffauswahlproxymodel.h"

extern Brauhelfer* bh;
extern Settings* gSettings;

WdgHopfenGabe::WdgHopfenGabe(int index, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WdgHopfenGabe),
    mIndex(index),
    mEnabled(true)
{
    ui->setupUi(this);

    ui->frameColor->setToolTip(tr("<b>Typ<br>"
                                  "<font color=\"%1\">Aroma</font><br>"
                                  "<font color=\"%2\">Bitter</font><br>"
                                  "<font color=\"%3\">Universal</font></b>")
            .arg(gSettings->HopfenTypBackgrounds[1].name())
            .arg(gSettings->HopfenTypBackgrounds[2].name())
            .arg(gSettings->HopfenTypBackgrounds[3].name()));

    ui->btnMengeKorrektur->setPalette(gSettings->paletteErrorButton);
    ui->btnAnteilKorrektur->setPalette(gSettings->paletteErrorButton);
    ui->btnMengeKorrektur->setVisible(false);
    ui->btnAnteilKorrektur->setVisible(false);

    checkEnabled(true);
    updateValues();
    connect(bh, SIGNAL(discarded()), this, SLOT(updateValues()));
    connect(bh->sud()->modelHopfengaben(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&, const QVector<int>&)), this, SLOT(updateValues()));
}

WdgHopfenGabe::~WdgHopfenGabe()
{
    delete ui;
}

bool WdgHopfenGabe::isEnabled() const
{
    return mEnabled;
}

QVariant WdgHopfenGabe::data(const QString &fieldName) const
{
    return bh->sud()->modelHopfengaben()->data(mIndex, fieldName);
}

bool WdgHopfenGabe::setData(const QString &fieldName, const QVariant &value)
{
    return bh->sud()->modelHopfengaben()->setData(mIndex, fieldName, value);
}

void WdgHopfenGabe::checkEnabled(bool force)
{
    bool enabled = !bh->sud()->getBierWurdeGebraut();
    if (enabled == mEnabled && !force)
        return;

    mEnabled = enabled;
    if (mEnabled)
    {
        RohstoffAuswahlProxyModel* model = new RohstoffAuswahlProxyModel(ui->cbZutat);
        model->setSourceModel(bh->modelHopfen());
        model->setColumnMenge(bh->modelHopfen()->fieldIndex("Menge"));
        model->setIndexMengeBenoetigt(bh->sud()->modelHopfengaben()->index(mIndex, bh->sud()->modelHopfengaben()->fieldIndex("erg_Menge")));
        ui->cbZutat->setModel(model);
        ui->cbZutat->setModelColumn(bh->modelHopfen()->fieldIndex("Beschreibung"));
        ui->cbZutat->setEnabled(true);
        ui->cbZutat->setCurrentIndex(-1);
        ui->btnLoeschen->setVisible(true);
        ui->tbVorhanden->setVisible(true);
        ui->lblVorhanden->setVisible(true);
        ui->lblEinheit->setVisible(true);
        ui->tbMenge->setReadOnly(false);
        ui->tbMengeProzent->setReadOnly(false);
        ui->tbAnteilProzent->setReadOnly(false);
        ui->tbKochdauer->setReadOnly(false);
        ui->cbVwh->setEnabled(true);
    }
    else
    {
        QStandardItemModel *model = new QStandardItemModel(ui->cbZutat);
        model->setItem(0, 0, new QStandardItem(data("Name").toString()));
        ui->cbZutat->setModel(model);
        ui->cbZutat->setModelColumn(0);
        ui->cbZutat->setEnabled(false);
        ui->cbZutat->setCurrentIndex(-1);
        ui->btnLoeschen->setVisible(false);
        ui->tbVorhanden->setVisible(false);
        ui->lblVorhanden->setVisible(false);
        ui->lblEinheit->setVisible(false);
        ui->tbMenge->setReadOnly(true);
        ui->tbMengeProzent->setReadOnly(true);
        ui->tbAnteilProzent->setReadOnly(true);
        ui->tbKochdauer->setReadOnly(true);
        ui->cbVwh->setEnabled(false);
        ui->btnAnteilKorrektur->setVisible(false);
        ui->btnMengeKorrektur->setVisible(false);
    }
}

bool WdgHopfenGabe::prozentIbu() const
{
    return bh->sud()->getberechnungsArtHopfen() == Hopfen_Berechnung_IBU;
}
//#include <QDebug>
void WdgHopfenGabe::updateValues()
{
    QString name = data("Name").toString();
//qDebug() << "isEnabled hops: " << ui->tbMengeProzent->isEnabled();
    checkEnabled(false);

    if (!ui->cbZutat->hasFocus())
    {
        ui->cbZutat->setCurrentIndex(-1);
        ui->cbZutat->setCurrentText(name);
    }
    if (!ui->tbMengeProzent->hasFocus())
        ui->tbMengeProzent->setValue(data("Prozent").toDouble());
    if (!ui->tbAnteilProzent->hasFocus())
        ui->tbAnteilProzent->setValue(data("Prozent").toDouble());
    if (!ui->tbMenge->hasFocus())
        ui->tbMenge->setValue(data("erg_Menge").toInt());
    if (!ui->tbAnteil->hasFocus())
        ui->tbAnteil->setValue(data("IBUAnteil").toDouble());
    ui->tbAlpha->setValue(data("Alpha").toDouble());
    ui->tbAusbeute->setValue(data("Ausbeute").toDouble());
    ui->cbVwh->setChecked(data("Vorderwuerze").toBool());
    if (!ui->tbKochdauer->hasFocus())
    {
        ui->tbKochdauer->setMinimum(-bh->sud()->getNachisomerisierungszeit());
        ui->tbKochdauer->setMaximum(bh->sud()->getKochdauerNachBitterhopfung());
        ui->tbKochdauer->setValue(data("Zeit").toInt());
    }

    int idx = bh->modelHopfen()->getValueFromSameRow("Beschreibung", name, "Typ").toInt();
    if (idx >= 0 && idx < gSettings->HopfenTypBackgrounds.count())
    {
        QPalette pal = ui->frameColor->palette();
        pal.setColor(QPalette::Background, gSettings->HopfenTypBackgrounds[idx]);
        ui->frameColor->setPalette(pal);
    }
    else
    {
        ui->frameColor->setPalette(gSettings->palette);
    }

    if (mEnabled)
    {
        ui->tbVorhanden->setValue(bh->modelHopfen()->getValueFromSameRow("Beschreibung", name, "Menge").toInt());
        int benoetigt = 0;
        ProxyModel* model = bh->sud()->modelHopfengaben();
        for (int i = 0; i < model->rowCount(); ++i)
        {
            if (model->data(i, "Name").toString() == name)
                benoetigt += model->data(i, "erg_Menge").toInt();
        }
        ui->tbVorhanden->setError(benoetigt > ui->tbVorhanden->value());

        ui->tbKochdauer->setReadOnly(ui->cbVwh->isChecked());

        if (prozentIbu())
        {
            ui->tbMenge->setReadOnly(false);
            ui->btnMengeKorrektur->setVisible(false);
            ui->tbMengeProzent->setVisible(false);
            ui->tbAnteilProzent->setVisible(true);
            ui->lblMengeProzent->setVisible(false);
            ui->lblAnteilProzent->setVisible(true);
        }
        else
        {
            ui->tbMenge->setReadOnly(true);
            ui->btnAnteilKorrektur->setVisible(false);
            ui->tbAnteilProzent->setVisible(false);
            ui->tbMengeProzent->setVisible(true);
            ui->lblAnteilProzent->setVisible(false);
            ui->lblMengeProzent->setVisible(true);
        }
    }
}

void WdgHopfenGabe::on_cbZutat_currentIndexChanged(const QString &text)
{
    if (ui->cbZutat->hasFocus())
        setData("Name", text);
}

void WdgHopfenGabe::on_tbMengeProzent_valueChanged(double value)
{
    if (ui->tbMengeProzent->hasFocus())
        setData("Prozent", value);
}

void WdgHopfenGabe::on_tbAnteilProzent_valueChanged(double value)
{
    if (ui->tbAnteilProzent->hasFocus())
        setData("Prozent", value);
}

double WdgHopfenGabe::prozent() const
{
    return data("Prozent").toDouble();
}

void WdgHopfenGabe::setFehlProzent(double value)
{
    if (mEnabled)
    {
        if (value < 0.0)
        {
            double p = prozent();
            if (p == 0.0)
                value = 0.0;
            else if (value < -p)
                value = -p;
        }
        QString text = (value < 0.0 ? "" : "+") + QLocale().toString(value, 'f', 1) + " %";
        ui->btnMengeKorrektur->setText(text);
        ui->btnMengeKorrektur->setProperty("toadd", value);
        ui->btnAnteilKorrektur->setText(text);
        ui->btnAnteilKorrektur->setProperty("toadd", value);
        if (prozentIbu())
            ui->btnAnteilKorrektur->setVisible(value != 0.0);
        else
            ui->btnMengeKorrektur->setVisible(value != 0.0);
    }
    else
    {
        ui->btnAnteilKorrektur->setVisible(false);
        ui->btnMengeKorrektur->setVisible(false);
    }
}

void WdgHopfenGabe::on_btnMengeKorrektur_clicked()
{
    setFocus();
    double toadd = ui->btnMengeKorrektur->property("toadd").toDouble();
    setData("Prozent", prozent() + toadd);
}

void WdgHopfenGabe::on_btnAnteilKorrektur_clicked()
{
    setFocus();
    double toadd = ui->btnAnteilKorrektur->property("toadd").toDouble();
    setData("Prozent", prozent() + toadd);
}

void WdgHopfenGabe::on_tbMenge_valueChanged(int value)
{
    if (ui->tbMenge->hasFocus())
        setData("erg_Menge", value);
}

void WdgHopfenGabe::on_tbKochdauer_valueChanged(int dauer)
{
    if (ui->tbKochdauer->hasFocus())
        setData("Zeit", dauer);
}

void WdgHopfenGabe::on_cbVwh_clicked(bool checked)
{
    setData("Vorderwuerze", checked);
}

void WdgHopfenGabe::on_btnLoeschen_clicked()
{
    bh->sud()->modelHopfengaben()->removeRow(mIndex);
}
