#!/bin/zsh

cd "$(dirname "$0")"  # Ir al directorio donde está el script

log_file="$PWD/push.log"
timestamp="$(date '+%Y-%m-%d %H:%M:%S')"

{
  echo "========== $timestamp =========="

  git add .

  if git diff --cached --quiet; then
    echo "[$timestamp] No hay cambios para commitear."
    notify-send "Git Auto Push" "No hay cambios para commitear."
    exit 0
  fi

  echo -n "Mensaje de commit (dejar vacío para 'Auto-commit'): "
  read commit_message

  if [[ -z "$commit_message" ]]; then
    commit_message="Auto-commit: $timestamp"
  fi

  echo "Haciendo commit..."
  git commit -m "$commit_message"

  echo "Haciendo push..."
  if git push -u origin main; then
    echo "[$timestamp] ✅ Push exitoso"
    notify-send "Git Auto Push" "✅ Push realizado con éxito"
  else
    echo "[$timestamp] ❌ Error al hacer push"
    notify-send "Git Auto Push" "❌ Error al hacer push"
    exit 1
  fi

} | tee -a "$log_file"
git status > /dev/null



