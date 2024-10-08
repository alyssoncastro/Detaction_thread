# Detecção de Faces com ESP32 CAM e FreeRTOS
---

###### ALYSSON C. C. CORDEIRO - ENGENHARIA DA COMPUTAÇÃO - INTELI

---

Este projeto é uma continuação e complemento da atividade anterior de detecção de faces com o ESP32 CAM. Agora, o sistema foi incrementado com o uso de FreeRTOS, que permite a divisão das tarefas em diferentes threads para aquisição, envio e recebimento de dados, proporcionando um controle mais eficiente e robusto do fluxo de informações.

A atividade anterior pode ser encontrada no seguinte repositório: [Detecção de Faces com ESP32 CAM](https://github.com/alyssoncastro/detection).

## Descrição do Projeto

O projeto utiliza o ESP32 CAM para capturar imagens e detectar faces em tempo real. A implementação foi dividida em três threads principais utilizando o FreeRTOS:

1. **Thread de Aquisição de Imagens**:
   - Responsável pela captura das imagens utilizando a câmera do ESP32 CAM.
   - A thread utiliza um semáforo binário para indicar quando uma nova imagem está pronta para ser processada.

2. **Thread de Envio de Imagens**:
   - Envia as imagens capturadas para um servidor Flask onde ocorre o processamento da imagem.
   - Esta thread só envia as imagens quando há uma nova captura, sincronizada com o semáforo da thread de aquisição.

3. **Thread de Recebimento de Detecção**:
   - Monitora a entrada serial para receber as coordenadas das faces detectadas enviadas pelo servidor.
   - Exibe as coordenadas das faces no terminal.

## Tecnologias Utilizadas

- **ESP32 CAM**
- **FreeRTOS**
- **Flask**
- **OpenCV**
- **Arduino IDE**

## Como Executar

### Pré-requisitos

- ESP32 CAM com os drivers corretamente instalados.
- Ambiente configurado com Arduino IDE e suporte ao ESP32.
- Flask e OpenCV instalados no servidor.

### Passos

1. **Configurar o ESP32 CAM**:
   - Faça o upload do código do ESP32 CAM utilizando o Arduino IDE.
   - Certifique-se de que os pinos estão corretamente configurados de acordo com o modelo da câmera.

2. **Executar o Servidor Flask**:
   - No servidor, execute o script Python que inicia o Flask e processa as imagens recebidas.
   
   ```bash
   cd/Detaction_thread/servidor_post/
   
   python main.py
   ```

### 3. Conectar o ESP32 CAM à Rede

- Certifique-se de que o ESP32 CAM está conectado à rede Wi-Fi correta.
- Verifique o código no ESP32 CAM e atualize o SSID e a senha da rede para a qual ele deve se conectar.
  
```cpp
const char *ssid = "Seu_SSID";     // Nome da rede Wi-Fi
const char *password = "Sua_Senha"; // Senha da rede Wi-Fi
```

- Confirme que o endereço IP do servidor Flask está correto no código do ESP32 CAM.

```cpp
const char* serverUrl = "http://<IP_DO_SERVIDOR_FLASK>:5000/upload";
```

- Faça o upload do código no ESP32 CAM e verifique o monitor serial para confirmar a conexão Wi-Fi.

### 4. Resolução de Problemas

Durante o desenvolvimento, teve alguns erros, como por exemplo:

1. **Erro na Inicialização da Câmera**:

- Verifique se os pinos da câmera estão configurados corretamente no código.
- Certifique-se de que a câmera está conectada corretamente ao ESP32 CAM. Uma conexão frouxa pode causar o erro de inicialização.

2. **Imagem Não Enviada**:

- Verifique se o ESP32 CAM está conectado à rede Wi-Fi correta.
- Confirme se o servidor Flask está em execução no IP correto e na porta 5000.
- Teste a conectividade entre o ESP32 CAM e o servidor Flask utilizando o comando `ping`.

3. **Performance Lenta**:

- Aumentar a resolução da imagem pode afetar a taxa de envio e processamento. Se o sistema estiver lento, tente reduzir a resolução da imagem no código:

```cpp
config.frame_size = FRAMESIZE_QVGA;
```
### Demostração em vídeo:

https://drive.google.com/file/d/1Zm-DXh9nRVLCS1-l_u2BWDkNIezFl0uR/view?usp=sharing