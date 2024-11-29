# upstream1/Dockerfile

FROM python:3.9-slim

# 作業ディレクトリの設定
WORKDIR /app

# 必要なパッケージのインストール
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# アプリケーションコードのコピー
COPY app.py .

# ポート80を開放
EXPOSE 80

# コンテナ起動時にアプリケーションを実行
CMD ["python", "app.py"]
