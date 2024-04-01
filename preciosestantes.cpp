#include "preciosestantes.h"
#include "ui_preciosestantes.h"
#include <QMessageBox>
#include <QSqlDatabase>
#include <QtSql>
#include <QFile>
#include <QSqlQuery>
#include <QtPrintSupport>
#include <QFontDatabase>
#include <QPainter>
#include <QTreeWidgetItem>
#include <QInputDialog>

PreciosEstantes::PreciosEstantes(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::PreciosEstantes)
{
    ui->setupUi(this);

    fuente_des.setCapitalization(QFont::AllUppercase);

    ui->combo_impresoras->addItems(QPrinterInfo::availablePrinterNames());
    ui->combo_impresoras->setCurrentText(QPrinterInfo::defaultPrinterName());

    set_previa("DESCRIPCION",0.0);
    update_fecha_db();

    ui->lista_cambios->addTopLevelItem(new QTreeWidgetItem(QStringList("Subieron")));
    ui->lista_cambios->addTopLevelItem(new QTreeWidgetItem(QStringList("Nuevos")));
    ui->lista_cambios->addTopLevelItem(new QTreeWidgetItem(QStringList("Bajaron")));

    ui->lista_cambios->setSelectionMode(QAbstractItemView::ExtendedSelection);

    set_previa("NO ENCONTRADO",0.0);

    ui->input->setFocus();
}

PreciosEstantes::~PreciosEstantes()
{
    delete ui;
}

bool PreciosEstantes::conectar_db(QSqlDatabase* db){
    QString ip = "192.168.0.105";
    QString port = "3306";
    QString base = "sicar";
    QString username = "consultas";
    QString password = "123456";

    QFile file("ip_server.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        ip = in.readLine();
        port = in.readLine();
        base = in.readLine();
        username = in.readLine();
        password = in.readLine();
        file.close();
    }

    db->setHostName(ip);
    db->setPort(port.toInt());
    db->setDatabaseName(base);
    db->setUserName(username);
    db->setPassword(password);

    if(!db->open()){
        QMessageBox messageBox;
        messageBox.critical(0,"Error","No se pudo conectar a la base de datos !");
        messageBox.setFixedSize(500,200);
        return false;
    }

    return true;
}

void PreciosEstantes::on_input_returnPressed()
{
    ui->input->selectAll();

    QSqlDatabase sicardb = QSqlDatabase::addDatabase("QMYSQL");
    if( !conectar_db(&sicardb) ){
        return;
    }

    QSqlQuery consulta;
    consulta.exec("SELECT descripcion, precio1, precio2, mayoreo2, precio3, mayoreo3, precio4, mayoreo4, impuesto FROM articulo a LEFT JOIN articuloimpuesto ai ON a.art_id=ai.art_id LEFT JOIN impuesto i ON i.imp_id=ai.imp_id WHERE clave='" + ui->input->text().toUpper() + "' OR claveAlterna='" + ui->input->text().toUpper() + "' AND a.status!=-1");
    qDebug() << consulta.lastError().text();

    if(consulta.next()) {

        float impuesto = 1;
        if( !consulta.value("impuesto").toString().isEmpty() ){
            impuesto = impuesto + consulta.value("impuesto").toFloat()/100.0;
        }

        set_previa(consulta.value("descripcion").toString().toUpper(),
                   consulta.value("precio1").toFloat() * impuesto,
                   consulta.value("precio2").toFloat() * impuesto,
                   consulta.value("mayoreo2").toFloat(),
                   consulta.value("precio3").toFloat() * impuesto,
                   consulta.value("mayoreo3").toFloat(),
                   consulta.value("precio4").toFloat() * impuesto,
                   consulta.value("mayoreo4").toFloat() );
    }else{
        set_previa("NO ENCONTRADO",0.0);
    }
}

void PreciosEstantes::set_previa(QString descripcion, float precio, float precio2, float cantidad2, float precio3, float cantidad3, float precio4, float cantidad4){

    descripcion = descripcion.toUpper();
    currentDesc = descripcion;
    currentPrice = precio;
    currentPrice2 = precio2;
    currentPrice3 = precio3;
    currentPrice4 = precio4;
    currentCant2 = cantidad2;
    currentCant3 = cantidad3;
    currentCant4 = cantidad4;

    QPixmap imagen(ancho, alto);
    QPainter documento;
    documento.begin(&imagen);
    documento.fillRect( QRect(0,0,ancho,alto), QColor(255,255,255) );

    escribir_etiqueta(&documento, descripcion, precio, precio2, cantidad2, precio3, cantidad3, precio4, cantidad4);

    ui->previa->setPixmap(imagen);

}

void PreciosEstantes::escribir_etiqueta(QPainter *documento, QString descripcion, float precio, float precio2, float cantidad2, float precio3, float cantidad3, float precio4, float cantidad4){

    QString num, num2;
    int checked_counter = 0;
    QString texto_precios = "";

    if(ui->segundo_precio->isChecked()){
        texto_precios = texto_precios + num.setNum(cantidad2,'g',3) + "pz " + "$" + num2.setNum(precio2,'f',2) + " @\n";
        checked_counter++;
    }
    if(ui->tercer_precio->isChecked()){
        texto_precios = texto_precios + num.setNum(cantidad3,'g',3) + "pz " + "$" + num2.setNum(precio3,'f',2) + " @\n";
        checked_counter++;
    }
    if(ui->cuarto_precio->isChecked()){
        texto_precios = texto_precios + num.setNum(cantidad4,'g',4) + "pz " + "$" + num2.setNum(precio4,'f',2) + " @\n";
        checked_counter++;
    }

    documento->setFont(fuente_des);
    documento->drawText( QRect(0, 5, ancho , 50), Qt::TextWordWrap | Qt::AlignHCenter | Qt::AlignVCenter , descripcion);

    QRect rect_precio_menudeo;
    QRect rect_precio_mayoreo;

    // ancho = 272; alto = 125;

    if(checked_counter == 3){

        fuente_precio.setStretch(QFont::Condensed);
        texto_precios = "A partir de: \n" + texto_precios;
        fuente_precio.setPointSize(30);
        fuente_precios_alt.setPointSize(9);
        int ancho_precio_main = 155;
        rect_precio_menudeo = QRect(0, 60, ancho_precio_main, 60);
        rect_precio_mayoreo = QRect(ancho_precio_main, 60, ancho-ancho_precio_main, 55);

    }else if(checked_counter == 2){

        fuente_precio.setStretch(QFont::Condensed);
        texto_precios = "A partir de: \n" + texto_precios;
        fuente_precio.setPointSize(32);
        fuente_precios_alt.setPointSize(10);
        int ancho_precio_main = 160;
        rect_precio_menudeo = QRect(0, 60, ancho_precio_main, 60);
        rect_precio_mayoreo = QRect(ancho_precio_main, 70, ancho-ancho_precio_main, 55);

    }else if(checked_counter == 1){

        fuente_precio.setStretch(82);
        texto_precios.replace("pz "," o más: \n");
        fuente_precio.setPointSize(35);
        fuente_precios_alt.setPointSize(11);
        int ancho_precio_main = 185;
        rect_precio_menudeo = QRect(0, 60, ancho_precio_main, 60);
        rect_precio_mayoreo = QRect(ancho_precio_main, 70, ancho-ancho_precio_main, 55);

    }else{

        texto_precios = "";
        fuente_precio.setPointSize(40);
        fuente_precios_alt.setPointSize(5);
        rect_precio_menudeo = QRect(0,65,ancho,60);
        rect_precio_mayoreo = QRect(0, 0, 0, 0);

    }

    documento->setFont(fuente_precio);
    documento->drawText( rect_precio_menudeo, Qt::AlignVCenter | Qt::AlignHCenter ,"$ " + num.setNum(precio,'f',2) );
    fuente_precio.setStretch(QFont::Unstretched);
    fuente_precio.setPointSize(35);

    documento->setFont(fuente_precios_alt);
    documento->drawText( rect_precio_mayoreo, Qt::AlignVCenter | Qt::AlignHCenter , texto_precios);
    fuente_precios_alt.setPointSize(11);

}

void PreciosEstantes::on_imprimir_previa_clicked()
{
    QPainter documento;
    QPrinter p = QPrinter(impresora_info,QPrinter::ScreenResolution);
    documento.begin(&p);

    documento.drawLine(0,1,ancho,1);
    documento.drawLine(0,2,ancho,2);

    escribir_etiqueta(&documento, currentDesc, currentPrice, currentPrice2, currentCant2, currentPrice3, currentCant3, currentPrice4, currentCant4);

    documento.end();
    ui->input->selectAll();
    ui->input->setFocus();
}

void PreciosEstantes::on_guardar_db_clicked()
{
    QString ruta = leer_ruta_db();

    QSqlDatabase sicardb = QSqlDatabase::addDatabase("QMYSQL");
    if( !conectar_db(&sicardb) ){
        return;
    }

    QFile::copy(ruta + ".back2",ruta + ".back3");
    QFile::copy(ruta + ".back1",ruta + ".back2");
    QFile::copy(ruta, ruta + ".back1");

    QFile file(ruta);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text)){
        QMessageBox messageBox;
        messageBox.critical(0,"Error","No se pudo escribir el archivo de base de datos !");
        messageBox.setFixedSize(500,200);
        return;
    }
    QTextStream out(&file);

    QSqlQuery consulta;
    consulta.exec("SELECT clave,precio1,impuesto FROM articulo a LEFT JOIN articuloimpuesto ai ON a.art_id=ai.art_id LEFT JOIN impuesto i ON i.imp_id=ai.imp_id WHERE a.status!=-1");

    while( consulta.next() ){
        float impuesto = 1;
        if( !consulta.value("impuesto").toString().isEmpty() ){
            impuesto = impuesto + consulta.value("impuesto").toFloat()/100.0;
        }

        QString id = consulta.value("clave").toString();
        float precio1_float = consulta.value("precio1").toFloat() * impuesto;
        QString precio1;
        precio1 = precio1.setNum(precio1_float,'g',10);
        out << id << "," << precio1 << "\n";
    }
    file.close();

    QMessageBox messageBox;
    messageBox.setText("Base de datos guardada correctamente.");
    messageBox.exec();

    update_fecha_db();
}

void PreciosEstantes::update_fecha_db(){

    QString ruta = leer_ruta_db();

    QFile file(ruta);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        ui->fecha_db->setText("N/A");
        return;
    }

    QFileInfo file_info(file);
    ui->fecha_db->setText(file_info.lastModified().toString("dd/MM/yyyy - h:m ap"));
    file.close();
}

void PreciosEstantes::on_combo_impresoras_currentIndexChanged(int index)
{
    impresora_info = QPrinterInfo::printerInfo(ui->combo_impresoras->currentText());
}

void PreciosEstantes::add_child(QStringList hijo, int pos){
    if(pos >= 0 && pos <= 2){
        QTreeWidgetItem* item_padre = ui->lista_cambios->topLevelItem(pos);
        QTreeWidgetItem* item_hijo = new QTreeWidgetItem(hijo);
        item_padre->addChild(item_hijo);
        item_padre->setExpanded(true);
    }
}

void PreciosEstantes::on_lista_cambios_itemClicked(QTreeWidgetItem *item, int column)
{
    if(item->text(1).length() > 0){
        ui->input->setText(item->text(0));
        on_input_returnPressed();
    }else{
        ui->input->setText(item->text(1));
        set_previa("DESCRIPCION", 0.0);
    }
}

void PreciosEstantes::on_borrar_lista_clicked()
{
    for(int i = 0; i < ui->lista_cambios->topLevelItemCount(); i++){
        ui->lista_cambios->topLevelItem(i)->takeChildren();
    }
}

void PreciosEstantes::on_consulta_todos_clicked()
{
    consultar(3);
}

void PreciosEstantes::on_consulta_subieron_clicked()
{
    consultar(0);
}

void PreciosEstantes::on_consulta_bajaron_clicked()
{
    consultar(2);
}

void PreciosEstantes::on_consulta_nuevos_clicked()
{
    consultar(1);
}

void PreciosEstantes::on_imprimir_todos_clicked()
{
    ui->progreso_nombre->setText("Imprimiendo todos ...");
    ui->progreso_barra->setValue(10);
    int tops = ui->lista_cambios->topLevelItemCount();
    for(int j = 0; j < ui->lista_cambios->topLevelItemCount(); j++){
        QList<QTreeWidgetItem*> items = ui->lista_cambios->topLevelItem(j)->takeChildren();
        int total = items.length();
        for(int i = 0; i < total; i++){
            if( items.at(i)->text(1)>0 ){
                ui->input->setText(items.at(i)->text(0));
                on_input_returnPressed();
                on_imprimir_previa_clicked();
                ui->lista_cambios->topLevelItem(j)->removeChild(items.at(i));
            }
            ui->progreso_barra->setValue((i+1)*(90/tops)/total + 10 + j*(90/tops));
        }
    }
    ui->progreso_barra->setValue(0);
    ui->progreso_nombre->setText("");
}

void PreciosEstantes::on_imprimir_seleccion_clicked()
{
    ui->progreso_nombre->setText("Imprimiendo seleccion ...");
    ui->progreso_barra->setValue(10);
    QList<QTreeWidgetItem*> seleccionados = ui->lista_cambios->selectedItems();
    for(int i = 0; i < seleccionados.length(); i++){
        if( seleccionados.at(i)->text(1)>0 ){
            ui->input->setText(seleccionados.at(i)->text(0));
            on_input_returnPressed();
            on_imprimir_previa_clicked();
            seleccionados.at(i)->parent()->removeChild(seleccionados.at(i));
        }
        ui->progreso_barra->setValue(i*90/seleccionados.length() + 10);
    }
    ui->progreso_barra->setValue(0);
    ui->progreso_nombre->setText("");
}

void PreciosEstantes::consultar(int modo){

    /// 3 = "Todos", 0 = "Subieron", 1 = "Nuevos" 2 = "Bajaron"

    ui->progreso_nombre->setText("Consultando ... ");
    ui->progreso_barra->setValue(10);

    QSqlDatabase sicardb = QSqlDatabase::addDatabase("QMYSQL");
    if( !conectar_db(&sicardb) ){
        ui->progreso_barra->setValue(0);
        ui->progreso_nombre->setText("");
        return;
    }

    on_borrar_lista_clicked();

    QSqlQuery consulta;
    consulta.exec("SELECT clave,descripcion,precio1,impuesto FROM articulo a LEFT JOIN articuloimpuesto ai ON a.art_id=ai.art_id LEFT JOIN impuesto i ON i.imp_id=ai.imp_id WHERE a.status!=-1");

    QString ruta = leer_ruta_db();

    QFile file(ruta);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox messageBox;
        messageBox.warning(0,"Ojo?","No se pudo abrir el archivo de base de datos! \nTodos los productos aparecerán como nuevos.");
        messageBox.setFixedSize(500,200);

        ui->progreso_barra->setValue(0);
        ui->progreso_nombre->setText("");

        int i = 0;
        int total = consulta.size();
        while( consulta.next() && (modo==1 || modo==3) ){

            float impuesto = 1;
            if( !consulta.value("impuesto").toString().isEmpty() ){ impuesto = impuesto + consulta.value("impuesto").toFloat()/100.0; }
            float precio1_float = consulta.value("precio1").toFloat() * impuesto;
            QString precio1;
            precio1 = precio1.setNum(precio1_float,'g',10);

            QStringList art;
            art << consulta.value("clave").toString();
            art << consulta.value("descripcion").toString();
            art << "N/A";
            art << precio1;
            add_child(art,1);
            ui->progreso_barra->setValue(i*90/total + 10);
            i++;
        }
        ui->progreso_barra->setValue(0);
        ui->progreso_nombre->setText("");
        return;
    }

    QTextStream in(&file);
    QStringList claves;
    QStringList precios;
    QString linea;

    while (!in.atEnd()) {
        linea = in.readLine();
        claves << linea.section(',',0,0);
        precios << linea.section(',',1,1);
    }
    file.close();

    ui->progreso_barra->setValue(30);
    int total = consulta.size();
    int i = 0;

    while(consulta.next()){

        float impuesto = 1;
        if( !consulta.value("impuesto").toString().isEmpty() ){ impuesto = impuesto + consulta.value("impuesto").toFloat()/100.0; }
        float precio1_float = consulta.value("precio1").toFloat() * impuesto;
        QString precio_art;
        precio_art = precio_art.setNum(precio1_float,'g',10);

        QString clave_art = consulta.value(0).toString();
        QString descripcion_art = consulta.value(1).toString();
        QString precio_snapshot;
        int index = claves.indexOf(clave_art);

        QStringList art = QStringList(clave_art);
        art << descripcion_art;

        if( index == -1 ){
            art << "N/A";
            art << "$" + precio_art.setNum(precio_art.toFloat(),'f',2);
            if(modo==1 || modo==3){
                add_child(art,1);
            }
        }else{
            precio_snapshot = precios.at(index);
            art << "$" + precio_snapshot.setNum(precio_snapshot.toFloat(),'f',2);
            art << "$" + precio_art.setNum(precio_art.toFloat(),'f',2);
            if( precio_art.toFloat() > precio_snapshot.toFloat() && (modo==0 || modo==3) ){
                add_child(art,0);
            }else if( precio_art.toFloat() < precio_snapshot.toFloat() && (modo==2 || modo==3) ){
                add_child(art,2);
            }
        }

        ui->progreso_barra->setValue(i*70/total + 30);
        i++;
    }
    ui->progreso_barra->setValue(0);
    ui->progreso_nombre->setText("");
}

QString PreciosEstantes::leer_ruta_db(){

    QFile file_ruta("ruta_database_snapshot.txt");
    if(!file_ruta.open(QIODevice::ReadOnly | QIODevice::Text)){
        QMessageBox messageBox;
        messageBox.critical(0,"Error","No se pudo abrir el archivo de ruta");
        messageBox.setFixedSize(500,200);
        ui->progreso_barra->setValue(0);
        ui->progreso_nombre->setText("");
        return "";
    }

    QTextStream in_ruta(&file_ruta);
    QString ruta = in_ruta.readLine();
    file_ruta.close();

    return ruta;
}

void PreciosEstantes::on_editar_nombre_clicked()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Editar Nombre ... ", "Escriba la descripcion", QLineEdit::Normal, currentDesc, &ok);
    if ( ok ){
        set_previa( text.toUpper(), currentPrice, currentPrice2, currentCant2, currentPrice3, currentCant3, currentPrice4, currentCant4);
    }
}

void PreciosEstantes::on_editar_precio_clicked()
{
    bool ok;
    float p = QInputDialog::getDouble(this,"Editar Precio ... ", "Escriba el precio (2 decimales)", currentPrice, 0.00, 100000.00, 2, &ok, Qt::WindowFlags(),0.50 );
    if( ok ){
        set_previa( currentDesc, p , currentPrice2, currentCant2, currentPrice3, currentCant3, currentPrice4, currentCant4);
    }
}

void PreciosEstantes::on_segundo_precio_stateChanged(int arg1)
{
    set_previa( currentDesc, currentPrice , currentPrice2, currentCant2, currentPrice3, currentCant3, currentPrice4, currentCant4);
}

void PreciosEstantes::on_tercer_precio_stateChanged(int arg1)
{
    set_previa( currentDesc, currentPrice , currentPrice2, currentCant2, currentPrice3, currentCant3, currentPrice4, currentCant4);
}

void PreciosEstantes::on_cuarto_precio_stateChanged(int arg1)
{
    set_previa( currentDesc, currentPrice , currentPrice2, currentCant2, currentPrice3, currentCant3, currentPrice4, currentCant4);
}

