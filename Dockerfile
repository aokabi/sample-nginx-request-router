# Dockerfile

FROM ubuntu:22.04 AS builder

# 必要なパッケージのインストール
RUN apt-get update && apt-get install -y \
    build-essential \
    libpcre3 \
    libpcre3-dev \
    zlib1g \
    zlib1g-dev \
    libssl-dev \
    wget \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Nginx のソースコードダウンロード
ENV NGINX_VERSION 1.27.2
WORKDIR /usr/src
RUN wget http://nginx.org/download/nginx-${NGINX_VERSION}.tar.gz \
    && tar -xzvf nginx-${NGINX_VERSION}.tar.gz \
    && rm nginx-${NGINX_VERSION}.tar.gz

# カスタムモジュールのソースをコピー
COPY ngx_http_route_module.c /usr/src/nginx-${NGINX_VERSION}/src/http/modules/
COPY config /usr/src/nginx-${NGINX_VERSION}/src/http/modules/

# Nginx をビルドして動的モジュールとしてコンパイル
WORKDIR /usr/src/nginx-${NGINX_VERSION}
RUN ./configure --with-compat --add-dynamic-module=src/http/modules \
    && make modules
    # && cp objs/ngx_http_route_module.so /etc/nginx/modules/ \
    # && make clean

# 動的モジュールを読み込む設定ファイルを作成
# RUN echo "load_module modules/ngx_http_hoge_route_module.so;" > /etc/nginx/modules-enabled/50-hoge-route.conf

FROM scratch AS export
COPY --from=builder /usr/src/nginx-1.27.2/objs/ngx_http_route_module.so ./