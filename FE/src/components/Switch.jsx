import './Switch.css';

export function Switch({ checked, onCheckedChange, className = "" }) {
  return (
    <button
      role="switch"
      aria-checked={checked}
      onClick={() => onCheckedChange(!checked)}
      className={`switch ${checked ? 'checked' : ''} ${className}`}
    >
      <span className="switch-thumb" />
    </button>
  );
}
