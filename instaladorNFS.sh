#!/bin/bash

set -e

# Colores
GREEN='\033[0;32m'
BLUE='\033[94m'
RED='\033[0;31m'
NC='\033[0m'

GAME_NAME="NFS"
EXEC_PATH="/usr/bin/$GAME_NAME"
CONFIG_PATH="/etc/$GAME_NAME"
DATA_PATH="/var/$GAME_NAME"
DESKTOP_PATH="$HOME/Desktop"
PROJECT_REPO="https://github.com/nicolascardone/TP-Taller-G7.git"
INSTALL_DIR="$HOME/NFS-juego"

# Función para verificar si un paquete está instalado
is_installed() {
    dpkg -s "$1" &> /dev/null
}

# Función para instalar paquete si no está presente
install_package() {
    local pkg=$1
    if is_installed "$pkg"; then
        echo -e "${BLUE}El paquete $pkg ya está instalado.${NC}"
    else
        echo -e "${GREEN}Instalando $pkg...${NC}"
        sudo apt install -y "$pkg" || { echo -e "${RED}Error instalando $pkg${NC}"; exit 1; }
    fi
}

echo -e "${GREEN}Actualizando repositorios...${NC}"
sudo apt update
sudo apt upgrade -y

echo -e "${GREEN}Instalando herramientas esenciales...${NC}"
install_package build-essential
install_package git

# Verificar CMake >= 3.24
REQUIRED_CMAKE="3.24.0"
current_cmake=$(cmake --version 2>/dev/null | head -n1 | awk '{print $3}' || echo "0")
if dpkg --compare-versions "$current_cmake" ge "$REQUIRED_CMAKE"; then
    echo -e "${BLUE}CMake $current_cmake ya está instalado.${NC}"
else
    echo -e "${GREEN}Instalando CMake >= $REQUIRED_CMAKE...${NC}"
    TMP_DIR=$(mktemp -d)
    cd "$TMP_DIR"
    CMAKE_VERSION="3.27.8"
    CMAKE_FILE="cmake-$CMAKE_VERSION-linux-x86_64.sh"
    wget https://github.com/Kitware/CMake/releases/download/v$CMAKE_VERSION/$CMAKE_FILE
    sudo sh $CMAKE_FILE --skip-license --prefix=/usr/local
    cd -
    rm -rf "$TMP_DIR"
    echo -e "${GREEN}CMake $CMAKE_VERSION instalado correctamente.${NC}"
fi

echo -e "${GREEN}Instalando Qt...${NC}"
install_package qt6-base-dev
install_package qt6-tools-dev
install_package qt6-tools-dev-tools

echo -e "${GREEN}Instalando SDL2 y librerías de fuentes...${NC}"
install_package libsdl2-dev
install_package libsdl2-image-dev
install_package libsdl2-mixer-dev
install_package libsdl2-ttf-dev
install_package libfreetype6-dev

echo -e "${GREEN}Instalando dependencias de audio para SDL2_mixer...${NC}"
install_package libflac-dev
install_package libvorbis-dev
install_package libmpg123-dev
install_package libopusfile-dev
install_package libxmp-dev
install_package libfluidsynth-dev
install_package libwavpack-dev
install_package libmikmod-dev
install_package libsmpeg-dev
install_package mpg321

echo -e "${GREEN}Instalando yaml-cpp...${NC}"
install_package libyaml-cpp-dev

echo -e "${GREEN}Creando carpetas para config y datos...${NC}"
sudo mkdir -p "$CONFIG_PATH"
sudo mkdir -p "$DATA_PATH"

echo -e "${GREEN}Copiando archivos YAML de ejemplo al directorio de configuración...${NC}"
if [[ -d ~/proyecto/settings ]]; then
    sudo cp ~/proyecto/settings/*.yaml "$CONFIG_PATH/" 2>/dev/null || true
    sudo chown $USER:$USER "$CONFIG_PATH/"*.yaml 2>/dev/null || true
    echo -e "${GREEN}Archivos YAML copiados y permisos ajustados.${NC}"
else
    echo -e "${RED}No se encontró la carpeta ~/proyecto/settings con archivos YAML.${NC}"
fi

# Clonar proyecto desde rama develop
if [[ ! -d "$INSTALL_DIR" ]]; then
    echo -e "${GREEN}Clonando proyecto desde GitHub...${NC}"
    git clone --branch develop "$PROJECT_REPO" "$INSTALL_DIR"
else
    echo -e "${BLUE}El proyecto ya existe en $INSTALL_DIR, se actualizará...${NC}"
    cd "$INSTALL_DIR"
    git fetch
    git checkout develop
    git pull origin develop
fi

cd "$INSTALL_DIR"

# Compilar solo si no está compilado
if [[ -f build/taller_client && -f build/taller_server ]]; then
    echo -e "${BLUE}Los ejecutables ya están compilados. Se saltará la compilación.${NC}"
else
    echo -e "${GREEN}Compilando proyecto...${NC}"
    mkdir -p build
    cd build
    cmake ..
    make -j$(nproc)
    cd ..
fi

# Detectar ejecutables generados
CLIENT_EXEC=$(find build -maxdepth 1 -type f -executable -name "taller_client*" | head -n1)
SERVER_EXEC=$(find build -maxdepth 1 -type f -executable -name "taller_server*" | head -n1)

if [[ -z "$CLIENT_EXEC" || -z "$SERVER_EXEC" ]]; then
    echo -e "${RED}No se encontraron los ejecutables en build/. Verifique la compilación.${NC}"
    exit 1
fi

# Copiar ejecutables a /usr/bin
echo -e "${GREEN}Moviendo ejecutables a /usr/bin...${NC}"
sudo cp "$CLIENT_EXEC" "$EXEC_PATH-client"
sudo cp "$SERVER_EXEC" "$EXEC_PATH-server"

# Crear scripts en la raíz del proyecto
echo -e "${GREEN}Creando scripts client.sh y server.sh en $INSTALL_DIR...${NC}"

cat <<EOL > "$INSTALL_DIR/client.sh"
#!/bin/bash
$EXEC_PATH-client 127.0.0.1 9090
EOL

cat <<EOL > "$INSTALL_DIR/server.sh"
#!/bin/bash
$EXEC_PATH-server 9090
EOL

chmod +x "$INSTALL_DIR/client.sh" "$INSTALL_DIR/server.sh"

# Crear accesos directos .desktop en Desktop
for SCRIPT in client.sh server.sh; do
    DESKTOP_FILE="$DESKTOP_PATH/$SCRIPT.desktop"
    cat <<EOL > "$DESKTOP_FILE"
[Desktop Entry]
Name=$SCRIPT
Exec=$INSTALL_DIR/$SCRIPT
Icon=$INSTALL_DIR/$SCRIPT.png
Type=Application
Terminal=true
EOL
    chmod +x "$DESKTOP_FILE"
done

echo -e "${GREEN}Instalación completada!${NC}"
echo -e "${BLUE}Ejecutables: $EXEC_PATH-client, $EXEC_PATH-server${NC}"
echo -e "${BLUE}Configuración: $CONFIG_PATH${NC}"
echo -e "${BLUE}Datos: $DATA_PATH${NC}"
echo -e "${BLUE}Scripts client.sh y server.sh en: $INSTALL_DIR${NC}"
echo -e "${BLUE}Accesos directos en Desktop.${NC}"

