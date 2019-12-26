## Sistema de Riego Automático

Es un proyecto final de la materia Taller de Proyecto I de la carrera Ingeniería en Computación de la UNLP. 

### **Grupo de Desarrollo:**
- Abba, Pedro Nicolás
- Ares, Charo
- Galán, Martín Andrés

### **Fecha**
  11/2/2018

### **1.- Introducción**

  El proyecto que se nos ha asignado como trabajo final de la materia ha sido la implementación de un sistema de riego para plantas en el hogar. La principal motivación para su creación es la automatización de una tarea tan periódica como es el riego de una o más plantas en el interior. 
  Teniendo esto en cuenta, nos planteamos realizar un sistema autónomo, que se configure al momento de su encendido y que pueda funcionar sin intervención de una persona por un tiempo específico. Para que este sea más interesante y que podamos aprovechar el potencial del microcontrolador que utilizaremos como “motor” central del proyecto, podremos modificar los parámetros ingresados al principio y agregaremos sensores para obtener datos de la planta y el suelo, y así informarlos al usuario a través de una interfaz visual simple e intuitiva.

### **2.- Objetivos**

  - Sensar la humedad de la tierra a través de un sensor de humedad
  - Poder ser capaces de regar la planta a través de un actuador
  - Configurar el sistema a través de un teclado matricial
  - Poder visualizar los datos en un LCD
  - Contar con un botón para arrancar/parar el sistema 
  - Agregar leds de control

### **3.- Propuesta**

  Nuestra propuesta es implementar un prototipo utilizando como plataforma de hardware la EDU-CIAA-NXP junto con un poncho de propia elaboración. Además estará alimentado por una fuente externa conectada a la red eléctrica para lograr un sistema más autónomo.
  El prototipo tendrá una estructura para obtener mayor robustez, portabilidad, simplicidad y atractivo. Además de tener una caja que contendrá tanto la EDU-CIAA-NXP como el poncho con todos sus componentes, tendrá un frasco a modo de tanque de agua y una planta en una maceta.
  Asimismo desarrollaremos un software para controlar todo lo referido al sistema de riego el cual estará implementado en la EDU-CIAA-NXP. El mismo contará con una interfaz de fácil comprensión para el usuario para lograr una experiencia más llevadera.

### **4.- Componentes**

  - EDU-CIAA NXP
  - Mini bomba de agua sumergible 5 Vol
  - Sensor de humedad de tierra
  - Fuente 5 Volts 1.5 Ampere
  - Poncho de propia implementación

### Para mas información leer el Informe del Proyecto
