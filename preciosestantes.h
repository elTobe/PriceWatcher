#ifndef PRECIOSESTANTES_H
#define PRECIOSESTANTES_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QPrinter>
#include <QPrinterInfo>
#include <QFontMetrics>
#include <QTreeWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class PreciosEstantes; }
QT_END_NAMESPACE

class PreciosEstantes : public QMainWindow
{
    Q_OBJECT

public:
    PreciosEstantes(QWidget *parent = nullptr);
    ~PreciosEstantes();

private slots:

    void on_input_returnPressed();

    void on_imprimir_previa_clicked();

    void on_guardar_db_clicked();

    void set_previa(QString desc, float precio, float precio2 = 0.0, float cantidad2 = 0.0, float precio3 = 0.0, float cantidad3 = 0.0, float precio4 = 0.0, float cantidad4 = 0.0);

    bool conectar_db(QSqlDatabase* db);

    void on_combo_impresoras_currentIndexChanged(int index);

    void update_fecha_db();

    void on_consulta_todos_clicked();

    void add_child(QStringList hijo, int pos);

    void on_lista_cambios_itemClicked(QTreeWidgetItem *item, int column);

    void on_borrar_lista_clicked();

    void on_consulta_subieron_clicked();

    void on_consulta_bajaron_clicked();

    void on_consulta_nuevos_clicked();

    void on_imprimir_todos_clicked();

    void on_imprimir_seleccion_clicked();

    void consultar(int modo);

    QString leer_ruta_db();

    void on_editar_nombre_clicked();

    void on_editar_precio_clicked();

    void escribir_etiqueta(QPainter *documento, QString descripcion, float precio, float precio2, float cantidad2, float precio3, float cantidad3, float precio4, float cantidad4);

    void on_segundo_precio_stateChanged(int arg1);

    void on_tercer_precio_stateChanged(int arg1);

    void on_cuarto_precio_stateChanged(int arg1);

private:

    Ui::PreciosEstantes *ui;

    QFont fuente_des = QFont("Sans Serif",16,QFont::Normal,false);
    QFont fuente_precio = QFont("Eras Bold ITC",40,QFont::Normal,false);
    QFont fuente_precios_alt = QFont("Sans Serif",11,QFont::Normal,false);

    QPrinterInfo impresora_info = QPrinterInfo::defaultPrinter();

    int ancho = 272;
    int alto = 125;

    QString currentDesc = "";
    float currentPrice = 0.0;
    float currentPrice2 = 0.0;
    float currentPrice3 = 0.0;
    float currentPrice4 = 0.0;
    float currentCant2 = 0.0;
    float currentCant3 = 0.0;
    float currentCant4 = 0.0;
};
#endif // PRECIOSESTANTES_H
