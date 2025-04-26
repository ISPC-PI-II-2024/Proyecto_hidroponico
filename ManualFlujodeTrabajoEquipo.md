https://github.com/ISPC-PI-II-2024/Proyecto_hidroponico.git
# 🛠️ Manual de trabajo en equipo con Git

## 📌 Objetivo
Organizar el desarrollo colaborativo de hardware y software usando un flujo de trabajo por ramas en Git. Cada integrante trabaja de forma aislada en su rama y se integran los cambios de forma progresiva.

---

## 🌳 Estructura de ramas

| Rama             | Propósito                                                          |
|------------------|--------------------------------------------------------------------|
| `main`           | Versión **estable y lista** para producción o entrega final.       |
| `development`    | Versión de desarrollo que reúne cambios funcionales para testeo.   |
| `feature-*`      | Ramas individuales de cada integrante o para tareas específicas.   |

Ejemplos:
- `feature-julian-sensorXXX`
- `feature-lucas-webapp`
- `feature-alina-componenteXXX`

---

## 🚀 Flujo de trabajo

### 1. Clonar el repositorio y crear tu rama de trabajo

```bash
git clone https://github.com/usuario/repositorio.git
cd repositorio
git checkout development
git pull origin development
git checkout -b feature-tu-nombre
```

---

### 2. Trabajar normalmente en tu rama

```bash
git add .
git commit -m "Mensaje claro del cambio realizado"
git push origin feature-tu-nombre
```

---

### 3. Crear un Pull Request (PR) hacia `develop`

- Desde GitHub: `Compare & Pull Request`
- Explicar qué hiciste, qué archivos modificaste, y si fue probado o no.
- Esperar revisión o aprobación.

---

### 4. Merge a `development`

- Se hace cuando el PR está aprobado y los cambios funcionan correctamente.

---

### 5. Cuando `development` está listo → Merge a `main`

- Esto lo realiza el profe o la persona encargada.
- Sucede cuando se entrega una versión final o estable.

---

## 🧹 Buenas prácticas

- Antes de subir cambios, sincronizá con `development`:
  ```bash
  git checkout development
  git pull origin development
  git checkout feature-tu-nombre
  git merge development
  ```

- No trabajar directamente sobre `main` ni `development`.
- Usar nombres descriptivos en los commits y las ramas.
- Mantener carpetas organizadas (`/hardware`, `/software`, `/docs`).
- Comentar el código y documentar lo importante.

---

## 📄 Plantilla para Pull Request

```markdown
### 🚀 ¿Qué se hizo?
- [x] Implementación de X
- [x] Pruebas iniciales
- [ ] Falta calibración

### 🧪 ¿Cómo se probó?
Se probó con lectura serial y comparación con valores de referencia.

### 📝 Notas
- Falta revisión de compatibilidad con el módulo Y.
```

---

Cualquier duda o mejora sobre el flujo, ¡se charla en equipo! 💬
