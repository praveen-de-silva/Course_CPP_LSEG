// Flower Exchange UI JavaScript

class FlowerExchangeUI {
    constructor() {
        this.forms = {};
        this.orders = new Set();
        this.reports = [];
        this.currentInstrument = 'Rose';
        this.orderBooks = {
            Rose: { buys: [], sells: [] },
            Lavender: { buys: [], sells: [] },
            Lotus: { buys: [], sells: [] },
            Tulip: { buys: [], sells: [] },
            Orchid: { buys: [], sells: [] }
        };

        this.init();
    }

    init() {
        this.setupEventListeners();
        this.updateStats();
        this.updateOrderBook();
    }

    setupEventListeners() {
        // Form elements
        this.forms.clientOrderId = document.getElementById('clientOrderId');
        this.forms.instrument = document.getElementById('instrument');
        this.forms.quantity = document.getElementById('quantity');
        this.forms.price = document.getElementById('price');

        // Side buttons
        document.querySelectorAll('.side-btn').forEach(btn => {
            btn.addEventListener('click', (e) => {
                document.querySelectorAll('.side-btn').forEach(b => b.classList.remove('active'));
                e.target.classList.add('active');
                this.updateSubmitButton();
            });
        });

        // Instrument tabs
        document.querySelectorAll('.instrument-tab').forEach(tab => {
            tab.addEventListener('click', (e) => {
                document.querySelectorAll('.instrument-tab').forEach(t => t.classList.remove('active'));
                e.target.classList.add('active');
                this.currentInstrument = e.target.dataset.instrument;
                this.updateCurrentInstrument();
                this.updateOrderBook();
            });
        });

        // Submit button
        document.getElementById('submitOrder').addEventListener('click', () => {
            this.submitOrder();
        });

        // Header buttons
        document.getElementById('importBtn').addEventListener('click', () => {
            document.getElementById('fileInput').click();
        });

        document.getElementById('exportBtn').addEventListener('click', () => {
            this.exportCSV();
        });

        document.getElementById('resetBtn').addEventListener('click', () => {
            this.resetExchange();
        });

        // File input
        document.getElementById('fileInput').addEventListener('change', (e) => {
            this.importCSV(e.target.files[0]);
        });

        // Instrument selector synchronization
        this.forms.instrument.addEventListener('change', (e) => {
            this.currentInstrument = e.target.value;
            this.updateCurrentInstrument();
            this.updateInstrumentTab();
            this.updateOrderBook();
        });

        // Enter key submission
        [this.forms.clientOrderId, this.forms.quantity, this.forms.price].forEach(input => {
            input.addEventListener('keydown', (e) => {
                if (e.key === 'Enter') {
                    this.submitOrder();
                }
            });
        });
    }

    updateSubmitButton() {
        const submitBtn = document.getElementById('submitOrder');
        const activeSide = document.querySelector('.side-btn.active');
        const side = activeSide ? activeSide.dataset.side : '1';

        if (side === '2') {
            submitBtn.classList.add('sell');
        } else {
            submitBtn.classList.remove('sell');
        }
    }

    updateCurrentInstrument() {
        document.getElementById('currentInstrument').textContent = this.currentInstrument.toUpperCase();
    }

    updateInstrumentTab() {
        document.querySelectorAll('.instrument-tab').forEach(tab => {
            tab.classList.toggle('active', tab.dataset.instrument === this.currentInstrument);
        });
    }

    async submitOrder() {
        const activeSide = document.querySelector('.side-btn.active');

        const orderData = {
            clientOrderId: this.forms.clientOrderId.value.trim(),
            instrument: this.currentInstrument,
            side: activeSide ? activeSide.dataset.side : '1',
            quantity: this.forms.quantity.value,
            price: this.forms.price.value
        };

        // Basic validation
        if (!orderData.clientOrderId) {
            this.notify('Client Order ID is required', '#ff4f6c');
            return;
        }

        if (!orderData.quantity || !orderData.price) {
            this.notify('Quantity and Price are required', '#ff4f6c');
            return;
        }

        try {
            const response = await fetch('/api/orders', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json'
                },
                body: JSON.stringify(orderData)
            });

            const result = await response.json();

            if (result.reports && result.reports.length > 0) {
                this.processReports(result.reports);
                this.clearForm();

                const report = result.reports[0];
                const statusText = this.getStatusText(report.status);

                if (report.status === 1) {
                    this.notify(`Rejected: ${report.reason}`, '#ff4f6c');
                } else if (report.status === 2 || report.status === 3) {
                    this.notify(`${result.reports.length} report(s) — ${statusText}`, '#00d4a0');
                } else {
                    this.notify('Order accepted → New', '#f4c030');
                }
            }
        } catch (error) {
            console.error('Error submitting order:', error);
            this.notify('Error submitting order', '#ff4f6c');
        }
    }

    processReports(newReports) {
        newReports.forEach(report => {
            this.reports.push(report);
            this.orders.add(report.orderId);
        });

        this.updateStats();
        this.updateReportsTable();
        this.updateOrderBook();
    }

    clearForm() {
        this.forms.clientOrderId.value = '';
        this.forms.quantity.value = '';
        this.forms.price.value = '';
    }

    updateStats() {
        const fills = this.reports.filter(r => r.status === 2).length;
        const pfills = this.reports.filter(r => r.status === 3).length;
        const rejected = this.reports.filter(r => r.status === 1).length;

        document.getElementById('orderCount').textContent = this.orders.size;
        document.getElementById('reportCount').textContent = this.reports.length;
        document.getElementById('fillCount').textContent = fills;
        document.getElementById('pfillCount').textContent = pfills;
        document.getElementById('rejectedCount').textContent = rejected;

        // Enable/disable export button
        document.getElementById('exportBtn').disabled = this.reports.length === 0;
    }

    updateReportsTable() {
        const tableBody = document.getElementById('reportsTableBody');

        if (this.reports.length === 0) {
            tableBody.innerHTML = '<div class="empty-message">No execution reports yet</div>';
            return;
        }

        const reportsHTML = [...this.reports].reverse().map(report => {
            const sideText = report.side === 1 ? 'BUY' : 'SELL';
            const sideClass = report.side === 1 ? '' : 'sell';
            const statusText = this.getStatusText(report.status);
            const statusClass = this.getStatusClass(report.status);

            return `
                <div class="report-row">
                    <div class="client-order-id">${report.clientOrderId}</div>
                    <div>${report.orderId}</div>
                    <div class="instrument ${report.instrument}">${report.instrument}</div>
                    <div class="side ${sideClass}">${sideText}</div>
                    <div><span class="status-badge ${statusClass}">${statusText}</span></div>
                    <div>${report.quantity}</div>
                    <div>${report.price.toFixed(2)}</div>
                    <div class="reason">${report.reason || ''}</div>
                </div>
            `;
        }).join('');

        tableBody.innerHTML = reportsHTML;
    }

    updateOrderBook() {
        const book = this.orderBooks[this.currentInstrument];
        const buyLevels = book.buys.length;
        const sellLevels = book.sells.length;

        document.getElementById('buyLevels').textContent = buyLevels;
        document.getElementById('sellLevels').textContent = sellLevels;

        const buySide = document.getElementById('buySide');
        const sellSide = document.getElementById('sellSide');
        const emptyMessage = document.getElementById('emptyBookMessage');
        const spreadValue = document.getElementById('spreadValue');

        // Show/hide empty message
        emptyMessage.style.display = (buyLevels === 0 && sellLevels === 0) ? 'block' : 'none';

        // Update spread
        if (buyLevels > 0 && sellLevels > 0) {
            const spread = (book.sells[0].price - book.buys[0].price).toFixed(2);
            spreadValue.textContent = `Δ ${spread}`;
        } else if (buyLevels === 0 && sellLevels === 0) {
            spreadValue.textContent = 'empty';
        } else {
            spreadValue.textContent = '—';
        }

        // Calculate max quantity for bar visualization
        const maxQty = Math.max(
            ...book.buys.map(b => b.qty),
            ...book.sells.map(s => s.qty),
            1
        );

        // Update sell side (reversed to show best offer at bottom)
        const sellHTML = [...book.sells].reverse().map(level => `
            <div class="order-level">
                <div></div>
                <div class="price-cell sell">${level.price.toFixed(2)}</div>
                <div class="sell-qty">
                    <div class="qty-bar">
                        <div class="qty-fill" style="width: ${(level.qty / maxQty) * 100}%"></div>
                    </div>
                    <span class="qty-text">${level.qty}</span>
                </div>
            </div>
        `).join('');

        // Update buy side
        const buyHTML = book.buys.map(level => `
            <div class="order-level">
                <div class="buy-qty">
                    <span class="qty-text">${level.qty}</span>
                    <div class="qty-bar">
                        <div class="qty-fill" style="width: ${(level.qty / maxQty) * 100}%"></div>
                    </div>
                </div>
                <div class="price-cell">${level.price.toFixed(2)}</div>
                <div></div>
            </div>
        `).join('');

        sellSide.innerHTML = sellHTML;
        buySide.innerHTML = buyHTML;
    }

    async importCSV(file) {
        if (!file) return;

        const text = await file.text();
        const lines = text.split(/\r?\n/).filter(line => line.trim());

        if (lines.length < 2) {
            this.notify('Invalid CSV format', '#ff4f6c');
            return;
        }

        // Process each order line (skip header)
        for (let i = 1; i < lines.length; i++) {
            const cols = lines[i].split(',').map(s => s.trim());

            if (cols.length >= 5) {
                const orderData = {
                    clientOrderId: cols[0],
                    instrument: cols[1],
                    side: cols[2],
                    quantity: cols[3],
                    price: cols[4]
                };

                try {
                    const response = await fetch('/api/orders', {
                        method: 'POST',
                        headers: { 'Content-Type': 'application/json' },
                        body: JSON.stringify(orderData)
                    });

                    const result = await response.json();
                    if (result.reports) {
                        this.processReports(result.reports);
                    }
                } catch (error) {
                    console.error('Error processing order:', error);
                }
            }
        }

        this.notify(`Imported ${lines.length - 1} orders → ${this.reports.length} reports`);
    }

    exportCSV() {
        if (this.reports.length === 0) return;

        const header = 'Client Order ID,Order ID,Instrument,Side,Exec Status,Quantity,Price,Reason,Transaction Time';
        const rows = this.reports.map(r => {
            const statusText = this.getStatusText(r.status);
            return `${r.clientOrderId},${r.orderId},${r.instrument},${r.side},${statusText},${r.quantity},${r.price.toFixed(2)},${r.reason},${r.transactionTime}`;
        });

        const csvContent = [header, ...rows].join('\n');
        const blob = new Blob([csvContent], { type: 'text/csv' });
        const url = URL.createObjectURL(blob);

        const a = document.createElement('a');
        a.href = url;
        a.download = 'execution_rep.csv';
        a.click();

        URL.revokeObjectURL(url);
    }

    async resetExchange() {
        try {
            await fetch('/api/reset', { method: 'POST' });

            this.orders.clear();
            this.reports = [];
            this.orderBooks = {
                Rose: { buys: [], sells: [] },
                Lavender: { buys: [], sells: [] },
                Lotus: { buys: [], sells: [] },
                Tulip: { buys: [], sells: [] },
                Orchid: { buys: [], sells: [] }
            };

            this.updateStats();
            this.updateReportsTable();
            this.updateOrderBook();
            this.notify('Exchange reset', '#3d5070');
        } catch (error) {
            console.error('Error resetting exchange:', error);
            this.notify('Error resetting exchange', '#ff4f6c');
        }
    }

    notify(message, color = '#00d4a0') {
        const notification = document.getElementById('notification');
        notification.textContent = message;
        notification.style.color = color;
        notification.classList.add('show');

        setTimeout(() => {
            notification.classList.remove('show');
        }, 2000);
    }

    getStatusText(status) {
        const statusMap = {
            0: 'New',
            1: 'Rejected',
            2: 'Fill',
            3: 'PFill'
        };
        return statusMap[status] || 'Unknown';
    }

    getStatusClass(status) {
        const classMap = {
            0: 'new',
            1: 'rejected',
            2: 'fill',
            3: 'pfill'
        };
        return classMap[status] || 'new';
    }
}

// Initialize the UI when the page loads
document.addEventListener('DOMContentLoaded', () => {
    new FlowerExchangeUI();
});