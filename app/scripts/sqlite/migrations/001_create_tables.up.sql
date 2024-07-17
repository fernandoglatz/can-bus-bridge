CREATE TABLE settings (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    key VARCHAR NOT NULL,
    value VARCHAR NOT NULL,
    created_at TIMESTAMP NOT NULL
);

CREATE TABLE preset (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    description VARCHAR NOT NULL,
    created_at TIMESTAMP NOT NULL
);

CREATE TABLE channel (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    preset_id INTEGER NOT NULL,
    description VARCHAR NOT NULL,
    pin INTEGER NOT NULL,
    channel INTEGER NOT NULL,
    baudrate INTEGER NOT NULL,
    clock INTEGER NOT NULL,
    readable BOOLEAN NOT NULL,
    reverse_mask BOOLEAN NOT NULL,
    enabled BOOLEAN NOT NULL,
    created_at TIMESTAMP NOT NULL,
    FOREIGN KEY(preset_id) REFERENCES preset(id)
);

CREATE TABLE channel_filter (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    channel_id INTEGER NOT NULL,
    extended BOOLEAN NOT NULL,
    mask INTEGER NOT NULL,
    filter INTEGER NOT NULL,
    enabled BOOLEAN NOT NULL,
    created_at TIMESTAMP NOT NULL,
    FOREIGN KEY(channel_id) REFERENCES channel(id)
);

CREATE TABLE pid_filter (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    channel_id INTEGER NOT NULL,
    pid INTEGER NOT NULL,
    enabled BOOLEAN NOT NULL,
    created_at TIMESTAMP NOT NULL,
    FOREIGN KEY(channel_id) REFERENCES channel(id)
);

CREATE TABLE transmission (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    interval INTEGER NOT NULL,
    pid INTEGER NOT NULL,
    bytes INTEGER NOT NULL,
    byte0 INTEGER NOT NULL,
    byte1 INTEGER NOT NULL,
    byte2 INTEGER NOT NULL,
    byte3 INTEGER NOT NULL,
    byte4 INTEGER NOT NULL,
    byte5 INTEGER NOT NULL,
    byte6 INTEGER NOT NULL,
    byte7 INTEGER NOT NULL,
    enabled BOOLEAN NOT NULL,
    created_at TIMESTAMP NOT NULL
);

CREATE TABLE trace (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    description VARCHAR NOT NULL,
    start_at TIMESTAMP NOT NULL,
    end_at TIMESTAMP,
    created_at TIMESTAMP NOT NULL
);

CREATE TABLE tracelog (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    trace_id INTEGER NOT NULL,
    channel VARCHAR NOT NULL,
    interval INTEGER NOT NULL,
    pid INTEGER NOT NULL,
    bytes INTEGER NOT NULL,
    byte0 INTEGER NOT NULL,
    byte1 INTEGER NOT NULL,
    byte2 INTEGER NOT NULL,
    byte3 INTEGER NOT NULL,
    byte4 INTEGER NOT NULL,
    byte5 INTEGER NOT NULL,
    byte6 INTEGER NOT NULL,
    byte7 INTEGER NOT NULL,
    created_at TIMESTAMP NOT NULL
);

CREATE TABLE pid_translate (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    pid INTEGER NOT NULL,
    bytes INTEGER NOT NULL,
    description VARCHAR NOT NULL,
    formula VARCHAR NOT NULL,
    created_at TIMESTAMP NOT NULL
);

CREATE TABLE bridge (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    preset_id INTEGER NOT NULL,
    channel_a INTEGER NOT NULL,
    channel_b INTEGER NOT NULL,
    pid_filter_mode INTEGER NOT NULL,
    bidirectional BOOLEAN NOT NULL,
    enabled BOOLEAN NOT NULL,
    created_at TIMESTAMP NOT NULL,
    FOREIGN KEY(preset_id) REFERENCES preset(id)
    FOREIGN KEY(channel_a) REFERENCES channel(id),
    FOREIGN KEY(channel_b) REFERENCES channel(id)
);

CREATE TABLE bridge_pid_filter (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    bridge_id INTEGER NOT NULL,
    pid INTEGER NOT NULL,
    created_at TIMESTAMP NOT NULL,
    FOREIGN KEY(bridge_id) REFERENCES bridge(id)
);

CREATE TABLE bridge_pid (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    preset_id INTEGER NOT NULL,
    pid INTEGER NOT NULL,
    channel_a INTEGER NOT NULL,
    channel_b INTEGER NOT NULL,
    bidirectional BOOLEAN NOT NULL,
    enabled BOOLEAN NOT NULL,
    created_at TIMESTAMP NOT NULL,
    FOREIGN KEY(preset_id) REFERENCES preset(id)
    FOREIGN KEY(channel_a) REFERENCES channel(id),
    FOREIGN KEY(channel_b) REFERENCES channel(id)
);

CREATE TABLE message_transform (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    bridge_pid INTEGER NOT NULL,
    from_byte INTEGER NOT NULL,
    to_byte INTEGER NOT NULL,
    created_at TIMESTAMP NOT NULL,
    FOREIGN KEY(bridge_pid) REFERENCES bridge_pid(id)
);

CREATE TABLE bomber (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    channel_id INTEGER NOT NULL,
    type INTEGER NOT NULL,
    message_interval INTEGER NOT NULL,
    increment_interval INTEGER NOT NULL,
    pid INTEGER NOT NULL,
    byte0 INTEGER NOT NULL,
    byte1 INTEGER NOT NULL,
    byte2 INTEGER NOT NULL,
    byte3 INTEGER NOT NULL,
    byte4 INTEGER NOT NULL,
    byte5 INTEGER NOT NULL,
    byte6 INTEGER NOT NULL,
    byte7 INTEGER NOT NULL,
    bomber_byte0 BOOLEAN NOT NULL,
    bomber_byte1 BOOLEAN NOT NULL,
    bomber_byte2 BOOLEAN NOT NULL,
    bomber_byte3 BOOLEAN NOT NULL,
    bomber_byte4 BOOLEAN NOT NULL,
    bomber_byte5 BOOLEAN NOT NULL,
    bomber_byte6 BOOLEAN NOT NULL,
    bomber_byte7 BOOLEAN NOT NULL,
    infinity BOOLEAN NOT NULL,
    enabled BOOLEAN NOT NULL,
    created_at TIMESTAMP NOT NULL,
    FOREIGN KEY(channel_id) REFERENCES channel(id)
);
