# PriceWatcher

![PriceWatcher](doc/img/screenshot1.png)

PriceWatcher es una aplicación desarrollada en Qt 5.15.2 que te permite monitorear cambios en el precio de productos para negocios que utilizan SICAR Punto de Venta. La aplicación muestra qué productos han subido de precio, cuáles han bajado y cuáles son nuevos en comparación con una snapshot previamente guardada de la base de datos.

## Requisitos

- Qt 5.15.2
- MySQL
- MinGW 8.1.0
- Impresora térmica de 80 mm

## Instalación

1. Clona este repositorio en tu máquina local.
2. Realiza la instalación de Qt 5.15.2 y MySQL en tu sistema.
3. Compila el proyecto abriendo el archivo `.pro` y ejecutándolo.
4. Si no deseas compilar desde el código fuente, ejecuta `windeployqt` en el archivo `PreciosEstantes.exe` ubicado en la ruta `doc/bin/` del repositorio.
5. Instala MinGW 8.1.0 en tu sistema.
6. Instala el setup `VC_redist` ubicado en la carpeta `doc/install_req/`.
7. Modifica el archivo `ip_server.txt` para configurar la IP del servidor donde se encuentra la base de datos de SICAR. Además, asegúrate de ingresar el usuario y la contraseña de SQL con permisos de consulta.

## Uso

Una vez instalado y configurado, ejecuta `PreciosEstantes.exe` para abrir la aplicación. La interfaz te permitirá realizar las siguientes acciones:

- Guardar una snapshot de precios de la base de datos.
- Comparar los cambios en los precios desde la snapshot guardada.
- Imprimir los indicadores de precio de los productos, ya sea uno por uno o en bulk, en una impresora térmica de 80 mm.

## Contribuciones

Las contribuciones son bienvenidas. Si deseas mejorar o agregar características a PriceWatcher, siéntete libre de hacer un fork del repositorio y enviar un pull request con tus cambios.

## Licencia

Este proyecto está bajo la Licencia MIT. Consulta el archivo `LICENSE` para obtener más detalles.
