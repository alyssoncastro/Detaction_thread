from flask import Flask, request, jsonify
import cv2
import numpy as np
import threading

app = Flask(__name__)

global img_to_display
img_to_display = None

# exibindo a imagem em uma janela
def display_image():
    global img_to_display
    while True:
        if img_to_display is not None:
            cv2.imshow('Imagem com Faces', img_to_display)
            if cv2.waitKey(1) & 0xFF == ord('q'): 
                break


thread = threading.Thread(target=display_image)
thread.start()

@app.route('/upload', methods=['POST'])
def upload_image():
    global img_to_display
    try:
        img_data = request.data
        np_img = np.frombuffer(img_data, np.uint8)
        img = cv2.imdecode(np_img, cv2.IMREAD_COLOR)

        if img is None:
            return jsonify({'error': 'Falha ao decodificar a imagem'}), 400

        
        face_cascade = cv2.CascadeClassifier('models/haarcascade_frontalface_default.xml')

        if face_cascade.empty():
            return jsonify({'error': 'Falha ao carregar o classificador de face'}), 500

        
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        
        faces = face_cascade.detectMultiScale(gray, 1.1, 4)

       
        if len(faces) == 0:
            img_to_display = img  
            cv2.putText(img, "Nenhuma face encontrada", (50, 50), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 255), 2, cv2.LINE_AA)
            cv2.imwrite('imagem_processada.jpg', img)  # Salva a imagem
            return jsonify({'status': 'Nenhuma face encontrada na imagem.'}), 200

        else:
            
            for (x, y, w, h) in faces:
                cv2.rectangle(img, (x, y), (x + w, y + h), (255, 0, 0), 2)

            
            img_to_display = img
            cv2.imwrite('imagem_processada.jpg', img)

            # Retornar as coordenadas das faces detectadas em JSON
            faces_coordinates = [{'x': int(x), 'y': int(y), 'w': int(w), 'h': int(h)} for (x, y, w, h) in faces]

            return jsonify({
                'status': f'{len(faces)} face(s) encontrada(s) e processada(s) com sucesso!',
                'faces': faces_coordinates
            }), 200

    except Exception as e:
        return jsonify({'error': f'Erro no processamento da imagem: {str(e)}'}), 500

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)
