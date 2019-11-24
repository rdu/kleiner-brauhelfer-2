#include "brauhelfer.h"
#include "database.h"

const int Brauhelfer::libVersionMajor = VER_MAJ;
const int Brauhelfer::libVerionMinor = VER_MIN;
const int Brauhelfer::libVersionPatch = VER_PAT;
const int Brauhelfer::supportedDatabaseVersion = libVersionMajor * 1000 + libVerionMinor;
const int Brauhelfer::supportedDatabaseVersionMinimal = 21;

Brauhelfer::Brauhelfer(const QString &databasePath, QObject *parent) :
    QObject(parent),
    mDatabasePath(databasePath),
    mReadonly(false)
{
    mDb = new Database();
    mDb->createTables(this);
    mSud = new SudObject(this);
    mCalc = new BierCalc();

    connect(mDb->modelSud, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelMalz, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelHopfen, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelHefe, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelWeitereZutaten, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelAusruestung, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelGeraete, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelWasser, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelRasten, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelMalzschuettung, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelHopfengaben, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelHefegaben, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelWeitereZutatenGaben, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelSchnellgaerverlauf, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelHauptgaerverlauf, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelNachgaerverlauf, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelBewertungen, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelAnhang, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelFlaschenlabel, SIGNAL(modified()), this, SIGNAL(modified()));
    connect(mDb->modelFlaschenlabelTags, SIGNAL(modified()), this, SIGNAL(modified()));
}

Brauhelfer::~Brauhelfer()
{
    disconnect();
    disconnectDatabase();
    delete mDb;
    delete mCalc;
    delete mSud;
}

bool Brauhelfer::connectDatabase()
{
    qInfo() << "Brauhelfer::connectDatabase():" << databasePath();
    if (isConnectedDatabase())
    {
        mSud->unload();
        mDb->disconnect();
    }
    if (mDb->connect(databasePath(), readonly()))
    {
        qInfo() << "Brauhelfer::connectDatabase() version:" << databaseVersion();
        if (mDb->version() == supportedDatabaseVersion)
        {
            mDb->select();
            mSud->init();
        }
    }
    emit connectionChanged(isConnectedDatabase());
    return isConnectedDatabase();
}

void Brauhelfer::disconnectDatabase()
{
    if (isConnectedDatabase())
    {
        qInfo("Brauhelfer::disconnectDatabase()");
        mSud->unload();
        mDb->disconnect();
        emit connectionChanged(isConnectedDatabase());
    }
}

bool Brauhelfer::isConnectedDatabase() const
{
    return mDb->isConnected();
}

bool Brauhelfer::readonly() const
{
    return mReadonly;
}

void Brauhelfer::setReadonly(bool readonly)
{
    if (mReadonly != readonly)
    {
        mReadonly = readonly;
        emit readonlyChanged(mReadonly);
    }
}

bool Brauhelfer::isDirty() const
{
    return mDb->isDirty();
}

void Brauhelfer::save()
{
    if (!readonly() && isDirty())
    {
        qInfo("Brauhelfer::save()");
        bool wasBlocked = blockSignals(true);
        mDb->save();
        blockSignals(wasBlocked);
        emit saved();
        emit modified();
    }
}

void Brauhelfer::discard()
{
    if (isDirty())
    {
        qInfo("Brauhelfer::discard()");
        bool wasBlocked = blockSignals(true);
        mDb->discard();
        blockSignals(wasBlocked);
        emit discarded();
        emit modified();
    }
}

QString Brauhelfer::databasePath() const
{
    return mDatabasePath;
}

void Brauhelfer::setDatabasePath(const QString &filePath)
{
    if (mDatabasePath != filePath)
    {
        mDatabasePath = filePath;
        emit databasePathChanged(mDatabasePath);
    }
}

int Brauhelfer::databaseVersion() const
{
    return mDb->version();
}

bool Brauhelfer::updateDatabase()
{
    qInfo("Brauhelfer::updateDatabase()");
    mDb->update();
    connectDatabase();
    return mDb->version() == supportedDatabaseVersion;
}

Database* Brauhelfer::db() const
{
    return mDb;
}

BierCalc* Brauhelfer::calc() const
{
    return mCalc;
}

SudObject* Brauhelfer::sud() const
{
    return mSud;
}

ModelSud *Brauhelfer::modelSud() const
{
    return mDb->modelSud;
}

ModelMalz *Brauhelfer::modelMalz() const
{
    return mDb->modelMalz;
}

ModelHopfen *Brauhelfer::modelHopfen() const
{
    return mDb->modelHopfen;
}

ModelHefe *Brauhelfer::modelHefe() const
{
    return mDb->modelHefe;
}

ModelWeitereZutaten *Brauhelfer::modelWeitereZutaten() const
{
    return mDb->modelWeitereZutaten;
}

ModelAusruestung *Brauhelfer::modelAusruestung() const
{
    return mDb->modelAusruestung;
}

SqlTableModel* Brauhelfer::modelGeraete() const
{
    return mDb->modelGeraete;
}

ModelWasser *Brauhelfer::modelWasser() const
{
    return mDb->modelWasser;
}

ModelRasten* Brauhelfer::modelRasten() const
{
    return mDb->modelRasten;
}

ModelMalzschuettung *Brauhelfer::modelMalzschuettung() const
{
    return mDb->modelMalzschuettung;
}

ModelHopfengaben* Brauhelfer::modelHopfengaben() const
{
    return mDb->modelHopfengaben;
}

ModelHefegaben* Brauhelfer::modelHefegaben() const
{
    return mDb->modelHefegaben;
}

ModelWeitereZutatenGaben *Brauhelfer::modelWeitereZutatenGaben() const
{
    return mDb->modelWeitereZutatenGaben;
}

ModelSchnellgaerverlauf *Brauhelfer::modelSchnellgaerverlauf() const
{
    return mDb->modelSchnellgaerverlauf;
}

ModelHauptgaerverlauf *Brauhelfer::modelHauptgaerverlauf() const
{
    return mDb->modelHauptgaerverlauf;
}

ModelNachgaerverlauf *Brauhelfer::modelNachgaerverlauf() const
{
    return mDb->modelNachgaerverlauf;
}

ModelBewertungen *Brauhelfer::modelBewertungen() const
{
    return mDb->modelBewertungen;
}

SqlTableModel* Brauhelfer::modelAnhang() const
{
    return mDb->modelAnhang;
}

SqlTableModel* Brauhelfer::modelFlaschenlabel() const
{
    return mDb->modelFlaschenlabel;
}

ModelFlaschenlabelTags *Brauhelfer::modelFlaschenlabelTags() const
{
    return mDb->modelFlaschenlabelTags;
}

int Brauhelfer::sudKopieren(int sudId, const QString& name, bool teilen)
{
    int row = modelSud()->getRowWithValue(ModelSud::ColID, sudId);
    if (row < 0)
        return -1;

    QMap<int, QVariant> values = modelSud()->copyValues(row);
    values.insert(ModelSud::ColSudname, name);
    if (!teilen)
    {
        values.insert(ModelSud::ColStatus, Sud_Status_Rezept);
        values.insert(ModelSud::ColMerklistenID, 0);
        values.insert(ModelSud::ColErstellt, QDateTime::currentDateTime().toString(Qt::ISODate));
        values.remove(ModelSud::ColBraudatum);
        values.remove(ModelSud::ColAbfuelldatum);
    }
    row = modelSud()->append(values);

    QVariant neueSudId = values.value(ModelSud::ColID);
    sudKopierenModel(modelRasten(), ModelRasten::ColSudID, sudId, {{ModelRasten::ColSudID, neueSudId}});
    sudKopierenModel(modelHopfengaben(), ModelHopfengaben::ColSudID, sudId, {{ModelHopfengaben::ColSudID, neueSudId}});
    if (teilen)
        sudKopierenModel(modelHefegaben(), ModelHefegaben::ColSudID, sudId, {{ModelHefegaben::ColSudID, neueSudId}});
    else
        sudKopierenModel(modelHefegaben(), ModelHefegaben::ColSudID, sudId, {{ModelHefegaben::ColSudID, neueSudId}, {ModelHefegaben::ColZugegeben, 0}});
    if (teilen)
        sudKopierenModel(modelWeitereZutatenGaben(), ModelWeitereZutatenGaben::ColSudID, sudId, {{ModelWeitereZutatenGaben::ColSudID, neueSudId}});
    else
        sudKopierenModel(modelWeitereZutatenGaben(), ModelWeitereZutatenGaben::ColSudID, sudId, {{ModelWeitereZutatenGaben::ColSudID, neueSudId}, {ModelWeitereZutatenGaben::ColZugabestatus, EWZ_Zugabestatus_nichtZugegeben}});
    sudKopierenModel(modelMalzschuettung(), ModelMalzschuettung::ColSudID, sudId, {{ModelMalzschuettung::ColSudID, neueSudId}});
    sudKopierenModel(modelAnhang(), ModelAnhang::ColSudID, sudId, {{ModelAnhang::ColSudID, neueSudId}});
    sudKopierenModel(modelFlaschenlabel(), ModelFlaschenlabel::ColSudID, sudId, {{ModelFlaschenlabel::ColSudID, neueSudId}});
    sudKopierenModel(modelFlaschenlabelTags(), ModelFlaschenlabelTags::ColSudID, sudId, {{ModelFlaschenlabelTags::ColSudID, neueSudId}});
    if (teilen)
    {
        sudKopierenModel(modelSchnellgaerverlauf(), ModelSchnellgaerverlauf::ColSudID, sudId, {{ModelSchnellgaerverlauf::ColSudID, neueSudId}});
        sudKopierenModel(modelHauptgaerverlauf(), ModelHauptgaerverlauf::ColSudID, sudId, {{ModelHauptgaerverlauf::ColSudID, neueSudId}});
        sudKopierenModel(modelNachgaerverlauf(), ModelNachgaerverlauf::ColSudID, sudId, {{ModelNachgaerverlauf::ColSudID, neueSudId}});
        sudKopierenModel(modelBewertungen(), ModelBewertungen::ColSudID, sudId, {{ModelBewertungen::ColSudID, neueSudId}});
    }
    return row;
}

void Brauhelfer::sudKopierenModel(SqlTableModel* model, int colSudId, const QVariant &sudId, const QMap<int, QVariant> &overrideValues)
{
    for (int r = 0; r < model->rowCount(); ++r)
    {
        if (model->data(r, colSudId) == sudId)
        {
            QMap<int, QVariant> values = model->copyValues(r);
            QMap<int, QVariant>::const_iterator it = overrideValues.constBegin();
            while (it != overrideValues.constEnd())
            {
                values.insert(it.key(), it.value());
                ++it;
            }
            model->append(values);
        }
    }
}

int  Brauhelfer::sudTeilen(int sudId, const QString& name1, const QString &name2, double prozent)
{
    int row1 = modelSud()->getRowWithValue(ModelSud::ColID, sudId);
    if (row1 < 0)
        return -1;
    int row2 = sudKopieren(sudId, name2, true);
    if (row2 < 0)
        return -1;

    double Menge = modelSud()->data(row1, ModelSud::ColMenge).toDouble();
    double WuerzemengeVorHopfenseihen = modelSud()->data(row1, ModelSud::ColWuerzemengeVorHopfenseihen).toDouble();
    double WuerzemengeKochende = modelSud()->data(row1, ModelSud::ColWuerzemengeKochende).toDouble();
    double Speisemenge = modelSud()->data(row1, ModelSud::ColSpeisemenge).toDouble();
    double WuerzemengeAnstellen = modelSud()->data(row1, ModelSud::ColWuerzemengeAnstellen).toDouble();
    double erg_AbgefuellteBiermenge = modelSud()->data(row1, ModelSud::Colerg_AbgefuellteBiermenge).toDouble();

    double factor = 1.0 - prozent;
    modelSud()->setData(row2, ModelSud::ColMenge, Menge * factor);
    modelSud()->setData(row2, ModelSud::ColWuerzemengeVorHopfenseihen, WuerzemengeVorHopfenseihen * factor);
    modelSud()->setData(row2, ModelSud::ColWuerzemengeKochende, WuerzemengeKochende * factor);
    modelSud()->setData(row2, ModelSud::ColSpeisemenge, Speisemenge * factor);
    modelSud()->setData(row2, ModelSud::ColWuerzemengeAnstellen, WuerzemengeAnstellen * factor);
    modelSud()->setData(row2, ModelSud::Colerg_AbgefuellteBiermenge, erg_AbgefuellteBiermenge * factor);
    int sudId2 = modelSud()->data(row2, ModelSud::ColID).toInt();
    for (int row = 0; row < modelHefegaben()->rowCount(); ++row)
    {
        if (modelHefegaben()->data(row, ModelHefegaben::ColSudID).toInt() == sudId2)
        {
            QModelIndex idx = modelHefegaben()->index(row, ModelHefegaben::ColMenge);
            modelHefegaben()->setData(idx, qRound(idx.data().toInt() * factor));
        }
    }

    factor = prozent;
    modelSud()->setData(row1, ModelSud::ColSudname, name1);
    modelSud()->setData(row1, ModelSud::ColMenge, Menge * factor);
    modelSud()->setData(row1, ModelSud::ColWuerzemengeVorHopfenseihen, WuerzemengeVorHopfenseihen * factor);
    modelSud()->setData(row1, ModelSud::ColWuerzemengeKochende, WuerzemengeKochende * factor);
    modelSud()->setData(row1, ModelSud::ColSpeisemenge, Speisemenge * factor);
    modelSud()->setData(row1, ModelSud::ColWuerzemengeAnstellen, WuerzemengeAnstellen * factor);
    modelSud()->setData(row1, ModelSud::Colerg_AbgefuellteBiermenge, erg_AbgefuellteBiermenge * factor);
    for (int row = 0; row < modelHefegaben()->rowCount(); ++row)
    {
        if (modelHefegaben()->data(row, ModelHefegaben::ColSudID).toInt() == sudId)
        {
            QModelIndex idx = modelHefegaben()->index(row, ModelHefegaben::ColMenge);
            modelHefegaben()->setData(idx, qRound(idx.data().toInt() * factor));
        }
    }

    return row1;
}
