// myapp/static/myapp/js/app.js
document.addEventListener('DOMContentLoaded', function() {
    loadTasks();
});

function loadTasks() {
    fetch('/api/tasks/')
        .then(response => response.json())
        .then(data => {
            const taskList = document.getElementById('task-list');
            taskList.innerHTML = '';
            data.forEach(task => {
                const taskItem = document.createElement('a');
                taskItem.href = '#';
                taskItem.className = 'list-group-item list-group-item-action';
                taskItem.innerHTML = `<strong>${task.title}</strong>: ${task.description}`;
                taskList.appendChild(taskItem);
            });
        })
        .catch(error => {
            console.error('Error fetching tasks:', error);
        });
}

function addTask() {
    const title = document.getElementById('title').value;
    const description = document.getElementById('description').value;

    fetch('/api/tasks/', {
        method: 'POST',
        headers: {
            'Content-Type': 'application/json'
        },
        body: JSON.stringify({ title, description })
    })
    .then(response => response.json())
    .then(data => {
        loadTasks();
        document.getElementById('title').value = '';
        document.getElementById('description').value = '';
    })
    .catch(error => {
        console.error('Error adding task:', error);
    });
}
