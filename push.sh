#!/bin/zsh

# Ir al directorio donde está el script (Raíz del repo)
cd "$(dirname "$0")"

log_file="$PWD/push.log"
timestamp="$(date '+%Y-%m-%d %H:%M:%S')"

# 1. Definir qué vamos a añadir (Argumento o todo)
# Si pasas un argumento ($1), usa eso. Si no, usa "."
target="${1:-.}"

{
  echo "========== $timestamp =========="

  # ==========================================
  # FASE 1: STAGE Y COMMIT
  # ==========================================
  echo "Añadiendo cambios en: $target"
  git add "$target"

  # Verificamos si hay algo en el área de stage listo para commitear
  if ! git diff --cached --quiet; then
    
    # Hay cambios nuevos -> Pedimos mensaje
    echo -n "Mensaje de commit (Enter para 'Auto-commit'): "
    read commit_message

    if [[ -z "$commit_message" ]]; then
      commit_message="Auto-commit: $timestamp"
    fi

    echo "Haciendo commit..."
    git commit -m "$commit_message"
    hizo_commit=true
  else
    echo "[$timestamp] No hay cambios nuevos para commitear."
    hizo_commit=false
  fi

  # ==========================================
  # FASE 2: PUSH (Si es necesario)
  # ==========================================
  
  # Verificamos si hay commits locales que no están en origin/main
  # Esto cuenta los commits que tienes tú pero que le faltan al server
  commits_pendientes=$(git log origin/main..HEAD --oneline 2>/dev/null | wc -l)

  if [[ "$commits_pendientes" -gt 0 ]]; then
    echo "Hay $commits_pendientes commits pendientes de subida."
    echo "Haciendo push..."
    
    if git push -u origin main; then
      echo "[$timestamp] ✅ Push exitoso"
      notify-send "Git Auto Push" "✅ Se subieron cambios ($commits_pendientes commits)"
    else
      echo "[$timestamp] ❌ Error al hacer push"
      notify-send "Git Auto Push" "❌ Error al hacer push"
      exit 1
    fi
  else
    # Si no hicimos commit ahora Y tampoco había pendientes de antes
    if [[ "$hizo_commit" == false ]]; then
        echo "[$timestamp] Nada que hacer (Todo limpio y sincronizado)."
        notify-send "Git Auto Push" "Nada nuevo que subir."
    else
        # Caso raro: Hicimos commit pero el contador de pendientes falló o algo
        # Intentamos push igual por si acaso si acabamos de commitear
        git push -u origin main
    fi
  fi

} | tee -a "$log_file"

# Limpieza visual
git status > /dev/null
